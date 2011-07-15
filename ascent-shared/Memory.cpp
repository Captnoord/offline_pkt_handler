#include "Common.h"

#include "../Tools/DbgHeap/DbgHeap.h"

//#ifndef DBG_HEAP_CHECK
//#define DBG_HEAP_CHECK
/*
void* operator new (size_t nSize)
{
    // the last parameter - allocation alignment method
    void* pPtr = g_Heap.Allocate(nSize, true);
    // you may run your program with both methods to ensure everything's ok

    if (!pPtr)
    {
        printf("unable to allocate heap\n");
        pPtr = g_Heap.Allocate(nSize, true);
        // do whatever you want if no memory. Either return NULL pointer
        // or throw an exception. This is flexible.
    }
    return pPtr;
}

void operator delete (void* pPtr)
{
    g_Heap.Free(pPtr);
}
*/

//#endif//DBG_HEAP_CHECK
