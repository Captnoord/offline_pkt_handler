// GuardObjs.h

// NOTE: the "pragma once" is not included in this file. Means - you can include it
// multiple times.
// 
// During the compilation this file tests which system header files were included so far, and
// declares guard objects only for those types.
#include "GObjBase.h"

#ifdef _DEBUG
#	ifndef ASSERT
#		define ASSERT(x) do { if (!(x)) __debugbreak(); } while (false)
#	endif
#	ifndef VERIFY
#		define VERIFY(x) ASSERT(x)
#	endif
#else // _DEBUG
#	ifndef ASSERT
#		define ASSERT(x) do {} while (false)
#	endif
#	ifndef VERIFY
#		define VERIFY(x) (x)
#	endif
#endif // _DEBUG


namespace GObj
{
// Specialized template classes, for specific guard types.

#if ((defined _WINDOWS_) && (!defined GOBJ_WIN32))
#	define GOBJ_WIN32

	// Encapsulates Win32 handles, that must be closed with CloseHandle
	struct GBaseH_HandleClose :public GBaseH_Core<HANDLE> {
		// Destruction via CloseHandle
		void InternalDestroy() { VERIFY(CloseHandle(m_Value)); }
	};
	// Base template class for Win32 HANDLE, destroyed by CloseHandle, NULL is invalid
	struct GBaseH_HandleCloseStd :public GBaseH_HandleClose, public GBaseH_Null<HANDLE> {
	};
	// Base template class for Win32 HANDLE, destroyed by CloseHandle, -1 is invalid
	struct GBaseH_HandleCloseFile :public GBaseH_HandleClose, public GBaseH_Minus1<HANDLE> {
	};
	// Base template class for Win32 HANDLE, destroyed by FindClose, -1 is invalid
	struct GBaseH_HandleFindClose :public GBaseH_Core<HANDLE>, public GBaseH_Minus1<HANDLE> {
		void InternalDestroy() { VERIFY(FindClose(m_Value)); }
	};
	// Final Guard for Win32 file handle.
	typedef GObj_T<GBaseH_HandleCloseFile>	HFile_G;
	// Final Guard for Win32 file find handle.
	typedef GObj_T<GBaseH_HandleFindClose>	HFileFind_G;
	// Final Guard for Win32 handle (other than file and file find handle).
	typedef GObj_T<GBaseH_HandleCloseStd>	Handle_G;
	// Base template class for Win32 HGLOBAL, destroyed by GlobalFree
	struct GBaseH_HGlobal : public GBaseH_CoreNull<HGLOBAL> {
		void InternalDestroy() { VERIFY(!GlobalFree(m_Value)); }
	};
	// Final Guard for Win32 HGLOBAL, destroyed by GlobalFree
	typedef GObj_T<GBaseH_HGlobal>		HGlobal_G;
	// Base template class for Win32 virtual memory pointer, destroyed by VirtualFree
	struct GBaseH_VMem : public GObj::GBaseH_CoreNull<PBYTE> {
		void InternalDestroy() { VERIFY(VirtualFree(m_Value, 0, MEM_RELEASE)); }
	};
	// Final Guard for Win32 virtual memory pointer, destroyed by VirtualFree with MEM_RELEASE flag
	typedef GObj_T<GBaseH_VMem> VMem_G;
	// Base template class for GDI objects, destroyed by DeleteObject
	struct GBaseH_GdiObj : public GBaseH_CoreNull<HGDIOBJ> {
		void InternalDestroy() { VERIFY(DeleteObject(m_Value)); }
	};
	// Final Guard for GDI objects, destroyed by DeleteObject
	typedef GObj_T<GBaseH_GdiObj>			HGdiObj_G;
	// Base template class for Win32 file-mapped pointer, destroyed by UnmapViewOfFile
	struct GBaseH_UnmapFile : public GBaseH_CoreNull<PBYTE> {
		void InternalDestroy() { VERIFY(UnmapViewOfFile(m_Value)); }
	};
	// Final Guard for Win32 file-mapped pointer, destroyed by UnmapViewOfFile
	typedef GObj_T<GBaseH_UnmapFile>		HFileMapping_G;
	// Base template class for Win32 module handle, destroyed by FreeLibrary
	struct GBaseH_Module : public GBaseH_CoreNull<HMODULE> {
		void InternalDestroy() { VERIFY(FreeLibrary(m_Value)); }
	};
	// Final Guard for Win32 module handle, destroyed by FreeLibrary
	typedef GObj_T<GBaseH_Module>			HModule_G;
	// Base template class for User32 window handle, destroyed by DestroyWindow
	struct GBaseH_DestroyWindow :public GBaseH_CoreNull<HWND> {
		void InternalDestroy() { VERIFY(DestroyWindow(m_Value)); }
	};
	// Final Guard for User32 window handle, destroyed by DestroyWindow
	typedef GObj_T<GBaseH_DestroyWindow> HWnd_G;

	// Base template class for OLE BSTR, destroyed by SysFreeString
	struct GBaseH_Bstr : public GBaseH_CoreNull<BSTR> {
		void InternalDestroy() { SysFreeString(m_Value); }
	};
	// Final Guard for OLE BSTR, destroyed by SysFreeString
	typedef GRef_T<GBaseH_Bstr>				Bstr_G;

	// Base template class for User32 HDC
	typedef GBaseH_CoreNull<HDC> GBaseH_Hdc;
	// Base template class for User32 HDC, destroyed by DeleteDC
	struct GBaseH_HdcDelete :public GBaseH_Hdc {
		void InternalDestroy() { VERIFY(DeleteDC(m_Value)); }
	};
	// Final Guard for User32 HDC, destroyed by DeleteDC
	typedef GObj_T<GBaseH_HdcDelete>		Hdc_G;

	// Base template class for User32 HDC, destroyed by ReleaseDC
	struct GBaseH_HdcRelease :public GBaseH_Hdc {
		HWND m_hWnd; // // The window for which the DC was acquired (by GetDC or similar). Will be passed into ReleaseDC
		void InternalDestroy() { ReleaseDC(m_hWnd, m_Value); }
	};
	// Final Guard for User32 HDC, destroyed by ReleaseDC
	class HWinDc_G :public GObj_T<GBaseH_HdcRelease> {
		INHERIT_GUARD_OBJ_BASE(HWinDc_G, GObj_T<GBaseH_HdcRelease>, HDC)
		// The window for the DC
		HWinDc_G(HWND hWnd) { m_hWnd = hWnd; }
	};

	// Base template class for User32 HDC, destroyed by EndPaint
	struct GBaseH_HdcEndPaint :public GBaseH_Hdc {
		HWND m_hWnd;
		PAINTSTRUCT m_PS;
		void InternalDestroy() { VERIFY(EndPaint(m_hWnd, &m_PS)); }
	};
	// Final Guard for User32 HDC, destroyed by EndPaint
	class HPaintDc_G :public GObj_T<GBaseH_HdcEndPaint> {
		INHERIT_GUARD_OBJ_BASE(HPaintDc_G, GObj_T<GBaseH_HdcEndPaint>, HDC)
		// Constructor, receives the target winfow. Both BeginPaint and EndPaint are to be called for this window
		HPaintDc_G(HWND hWnd) { m_hWnd = hWnd; }
		// Wraps the BeginPaint. Saves the returned DC, contains the filled PAINTSTRUCT
		bool Begin()
		{
			InternalCleanup();
			m_Value = BeginPaint(m_hWnd, &m_PS);
			return IsValid();
		}
	};

	// Base template class for GDI objects deselected from a DC.
	struct GBaseH_HdcSelect : public GBaseH_CoreNull<HGDIOBJ> {
		HDC m_hDC; // The DC to deselect object into
		void InternalDestroy() { VERIFY(SelectObject(m_hDC, m_Value)); }
	};
	// Final Guard for for GDI objects deselected from a DC. Destroyed by Selecting the object back into the DC
	class HSelectDc_G : public GObj_T<GBaseH_HdcSelect>
	{
		INHERIT_GUARD_OBJ_BASE(HSelectDc_G, GObj_T<GBaseH_HdcSelect>, HGDIOBJ)
		// Pass the DC at the constructor.
		HSelectDc_G(HDC hDC) { m_hDC = hDC; }
	};


#endif // GOBJ_WIN32

#if (((defined _WINSOCK2API_) || (defined _WINSOCKAPI_)) && (!defined GOBJ_SOCKET))
#	define GOBJ_SOCKET
	// Base template class for Winsock SOCKET, destroyed by closesocket, -1 is invalid
	struct GBaseH_Socket : public GBaseH_CoreMinus1<SOCKET> {
		void InternalDestroy() { VERIFY(!closesocket(m_Value)); }
	};
	// Final Guard for Winsock socket handle.
	typedef GObj_T<GBaseH_Socket>			Socket_G;

#endif // GOBJ_SOCKET

#if ((defined __WINCRYPT_H__) && (!defined GOBJ_WCRYPT))
#	define GOBJ_WCRYPT
	// Base template class for Windows HCRYPTHASH handle, destroyed by CryptDestroyHash
	struct GBaseH_CryptHash : public GBaseH_CoreNull<HCRYPTHASH> {
		void InternalDestroy() { VERIFY(CryptDestroyHash(m_Value)); }
	};
	// Base template class for Windows HCRYPTKEY handle, destroyed by CryptDestroyKey
	struct GBaseH_CryptKey : public GBaseH_CoreNull<HCRYPTKEY> {
		void InternalDestroy() { VERIFY(CryptDestroyKey(m_Value)); }
	};
	// Final Guard for Windows HCRYPTHASH handle, destroyed by CryptDestroyHash
	typedef GObj_T<GBaseH_CryptHash>		HCryptHash_G;
	// Final Guard for Windows HCRYPTKEY handle, destroyed by CryptDestroyKey
	typedef GObj_T<GBaseH_CryptKey>			HCryptKey_G;

#endif // GOBJ_WCRYPT

#if ((defined _INC_MMSYSTEM) && (!defined GOBJ_MMSYSTEM))
#	define GOBJ_MMSYSTEM

	// Base template class for WinMM HMMIO handle, destroyed by mmioClose
	struct GBaseH_HMMIO : public GObj::GBaseH_CoreNull<HMMIO> {
		void InternalDestroy() { VERIFY(!mmioClose(m_Value, 0)); }
	};
	// Final Guard for WinMM HMMIO handle, destroyed by mmioClose
	typedef GObj::GObj_T<GBaseH_HMMIO> HmmIo_G;

	// Base template class for WinMM HWAVEOUT handle, destroyed by waveOutClose
	struct GBaseH_HWAVEOUT : public GObj::GBaseH_CoreNull<HWAVEOUT> {
		void InternalDestroy() { VERIFY(!waveOutClose(m_Value)); }
	};
	// Final Guard for WinMM HWAVEOUT handle, destroyed by waveOutClose
	typedef GObj::GObj_T<GBaseH_HWAVEOUT> HWaveOut_G;

	// Base template class for WinMM HWAVEIN handle, destroyed by waveInClose
	struct GBaseH_HWAVEIN : public GObj::GBaseH_CoreNull<HWAVEIN> {
		void InternalDestroy() { VERIFY(!waveInClose(m_Value)); }
	};
	// Final Guard for WinMM HWAVEIN handle, destroyed by waveInClose
	typedef GObj::GObj_T<GBaseH_HWAVEIN> HWaveIn_G;

#endif // GOBJ_MMSYSTEM

#if ((defined _INC_ACM) && (!defined GOBJ_ACM))
#	define GOBJ_ACM

	// Base template class for WinMM HACMSTREAM handle, destroyed by acmStreamClose
	struct GBaseH_HACMSTREAM : public GBaseH_CoreNull<HACMSTREAM> {
		void InternalDestroy() { VERIFY(!acmStreamClose(m_Value, 0)); }
	};
	// Final Guard for WinMM HACMSTREAM handle, destroyed by acmStreamClose
	typedef GObj_T<GBaseH_HACMSTREAM> HacmStream_G;

#endif // GOBJ_ACM

#if ((defined _WININET_) && (!defined GOBJ_WININET))
#	define GOBJ_WININET

	// Base template class for Wininet HINTERNET handle, destroyed by InternetCloseHandle
	struct GBaseH_HINet : public GBaseH_CoreNull<HINTERNET> {
		void InternalDestroy() { VERIFY(InternetCloseHandle(m_Value)); }
	};
	// Final Guard for Wininet HINTERNET handle, destroyed by InternetCloseHandle
	typedef GObj_T<GBaseH_HINet>		HINet_G;

#endif // GOBJ_WININET

}; // namespace GObj

