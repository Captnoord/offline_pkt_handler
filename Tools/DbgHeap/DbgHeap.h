#pragma once
#include "GuardObjs.h"
#include "ContainersInl.h"

struct CriticalSection :public CRITICAL_SECTION {
	CriticalSection() { InitializeCriticalSection(this); }
	~CriticalSection() { DeleteCriticalSection(this); }

	class Scope {
		CRITICAL_SECTION& m_cs;
	public:
		Scope(CRITICAL_SECTION& cs) :m_cs(cs) { EnterCriticalSection(&cs); }
		~Scope() { LeaveCriticalSection(&m_cs); }
	};
};

class DbgHeap {

	DWORD m_dwPageSize;

	struct Reserved :public Container::NodeEx<Container::QueueHT, Reserved> {
		GObj::VMem_G m_pMem;
		size_t m_nSize;

		void* operator new (size_t nSize) {
			return HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, nSize);
		}
		void operator delete (void* pPtr) {
			HeapFree(GetProcessHeap(), 0, pPtr);
		}
	};

	Container::ListDyn<Container::QueueHT, Reserved> m_queReserved;
	friend class Container::Impl::InhDyn<Container::ListEx<Container::QueueHT, Reserved>, Reserved>;
	friend class Container::ListDyn<Container::QueueHT, Reserved>;

	CriticalSection m_cs;
	size_t m_nSizeReserved;

public:

	DbgHeap()
		:m_nSizeReserved(0)
	{
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		m_dwPageSize = sysInfo.dwPageSize;
	}

	void* Allocate(size_t nSize, bool bAlignTop = true)
	{
        /*if (!m_dwPageSize)
        {
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            m_dwPageSize = sysInfo.dwPageSize;
        }*/

		GObj::VMem_G pPtr = (PBYTE) VirtualAlloc(NULL, nSize + m_dwPageSize, MEM_RESERVE | MEM_LARGE_PAGES, PAGE_NOACCESS);
        if (!pPtr) {
            DWORD lError = GetLastError();
            printf("virtual alloc failed\n");
			return NULL;
        }

		PBYTE pRet = pPtr;
		if (bAlignTop)
		{
			DWORD dwExtra = (DWORD) nSize % m_dwPageSize;
			if (dwExtra)
				pRet += m_dwPageSize - dwExtra;
		} else
			pRet += m_dwPageSize;

		if (!VirtualAlloc(pRet, nSize, MEM_COMMIT, PAGE_READWRITE))
			return NULL;

		// ok
		pPtr.Detach();
		return pRet;
	}

	void Free(void* pPtr)
	{
		if (!pPtr)
			return;

		MEMORY_BASIC_INFORMATION mbi;
		VERIFY(VirtualQuery(pPtr, &mbi, sizeof(mbi)) == sizeof(mbi));

		GObj::VMem_G pVMem((PBYTE) mbi.AllocationBase);

		ASSERT(MEM_COMMIT == mbi.State);
		ASSERT(PAGE_READWRITE == mbi.Protect);

		size_t nReserveSize = mbi.RegionSize + m_dwPageSize;

		VERIFY(VirtualFree(pPtr, mbi.RegionSize, MEM_DECOMMIT));

		CriticalSection::Scope scope(m_cs);

		Reserved* pNode = m_queReserved.CreateTail();
		pNode->m_pMem.Attach(pVMem.Detach());
		pNode->m_nSize = nReserveSize;

		m_nSizeReserved += nReserveSize;

		while (m_nSizeReserved > 50 * 1024 * 1024)
		{
			ASSERT(m_queReserved._Head);
			ASSERT(pNode);

			m_nSizeReserved -= m_queReserved._Head->m_nSize;
			m_queReserved.DeleteHead();
		}
	}
};
