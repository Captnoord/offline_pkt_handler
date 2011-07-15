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

#ifndef _BUFFERPOOL_H
#define _BUFFERPOOL_H

/* buffer pool experiment */
class BufferPool : public Singleton<BufferPool>
{
public:
	BufferPool();
	~BufferPool();

	void init();

	void* GetBuffer(size_t size);
	void DumpBuffer(void* buffer, size_t size);
	void ResizeBuffer(void** buffer, size_t oldsize, size_t newsize, bool copy = false);
	void print_stats();
protected:
private:

	FastQueue<void*,DummyLock> mSmallPool; /* buffer from 0x0 - 0x100 */
	FastQueue<void*,DummyLock> mMediumPool; /* buffer from 0x100 - 0x1000 */
	FastQueue<void*,DummyLock> mBigPool; /* buffer from 0x1000 - 0xFFFFFFFF */

	size_t mAllocatedMem; // keeps track of the total allocated mem of the buffer pool.
	size_t mSmallRecycleCount;
	size_t mMediumRecycleCount;
	size_t mLargeRecycleCount;

	size_t mSmallBufferCount;
	size_t mMediumBufferCount;
	size_t mLargeBufferCount;
};

// buffer pool experiment
#define sBufferPool BufferPool::getSingleton()

#endif // _BUFFERPOOL_H