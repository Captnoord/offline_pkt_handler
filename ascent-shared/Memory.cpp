/*
	------------------------------------------------------------------------------------
	LICENSE:
	------------------------------------------------------------------------------------
	This file is part of EVEmu: EVE Online Server Emulator
	Copyright 2006 - 2011 The EVEmu Team
	For the latest information visit http://evemu.org
	------------------------------------------------------------------------------------
	This program is free software; you can redistribute it and/or modify it under
	the terms of the GNU Lesser General Public License as published by the Free Software
	Foundation; either version 2 of the License, or (at your option) any later
	version.

	This program is distributed in the hope that it will be useful, but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License along with
	this program; if not, write to the Free Software Foundation, Inc., 59 Temple
	Place - Suite 330, Boston, MA 02111-1307, USA, or go to
	http://www.gnu.org/copyleft/lesser.txt.
	------------------------------------------------------------------------------------
	Author:		Captnoord
*/

#include "Common.h"

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
