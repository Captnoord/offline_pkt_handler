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

#include "EvemuPCH.h"

createFileSingleton( BufferPool );

void* BufferPool::GetBuffer( size_t size )
{
	void *outbuff;
	if (size <= 0x100)
	{
		if (mSmallPool.GetSize() > 0)
		{
			outbuff = mSmallPool.Pop();
			//void* bufferEntry = mSmallPool.Pop();
			//assert(bufferEntry);
			//return bufferEntry;
		}
		else
		{
			mAllocatedMem+=0x100;
			//void* buffer = ASCENT_MALLOC(0x100);
			//assert(buffer);
			//return buffer;
			outbuff = ASCENT_MALLOC(0x100);
			mSmallBufferCount++;
			//assert(outbuff);
		}
	}
	else if (size > 0x100 && size <= 0x1000)
	{
		if (mMediumPool.GetSize() > 0)
		{
			outbuff = mMediumPool.Pop();
			//void* bufferEntry = mMediumPool.Pop();
			//assert(bufferEntry);
			//return bufferEntry;
		}
		else
		{
			mAllocatedMem+=0x1000;
			//void* buffer = ASCENT_MALLOC(0x1000);
			//assert(buffer);
			//return buffer;
			outbuff = ASCENT_MALLOC(0x1000);
			mMediumBufferCount++;
			//assert(outbuff);
		}
	}
	else if (size > 0x1000 && size < 0x100000)
	{
		if (mBigPool.GetSize() > 0)
		{
			//void* bufferEntry = mBigPool.Pop();
			//assert(bufferEntry);
			//return bufferEntry;
			outbuff = mBigPool.Pop();
		}
		else
		{
			mAllocatedMem+=0x100000;
			//void* buffer = ASCENT_MALLOC(0x100000);
			//assert(buffer);
			//return buffer;
			outbuff = ASCENT_MALLOC(0x100000);
			mLargeBufferCount++;
			//assert(outbuff);
		}
	}
	else
		ASCENT_HARDWARE_BREAKPOINT;

	//assert(outbuff);
	//printf("BufferPool give out: %p\n", outbuff);
	return outbuff;
}

BufferPool::BufferPool()
{
	mAllocatedMem = 0;
	mSmallRecycleCount = 0;
	mMediumRecycleCount = 0;
	mLargeRecycleCount = 0;

	mSmallBufferCount = 0;
	mMediumBufferCount = 0;
	mLargeBufferCount = 0;
}

BufferPool::~BufferPool()
{
	void * dig;
	while (dig = mSmallPool.Pop())
	{
		ASCENT_FREE(dig);	
	}

	while (dig = mMediumPool.Pop())
	{
		ASCENT_FREE(dig);	
	}

	while (dig = mBigPool.Pop())
	{
		ASCENT_FREE(dig);	
	}
}

void BufferPool::DumpBuffer( void* buffer, size_t size )
{
	//assert(buffer);
	//assert(size);
	//printf("BufferPool take back: %p\n", buffer);

	/* whoot lol*/
	if (size <= 0x100)
	{
		mSmallPool.Push(buffer);
		mSmallRecycleCount++;
	}
	else if (size > 0x100 && size <= 0x1000)
	{
		mMediumPool.Push(buffer);
		mMediumRecycleCount++;
	}
	else if (size > 0x1000 && size < 0x100000)
	{
		mBigPool.Push(buffer);
		mLargeRecycleCount++;
	}
	else
		ASCENT_HARDWARE_BREAKPOINT;
}

void BufferPool::print_stats()
{
	printf("BufferPool\n");
	printf("  Allocated: %u bytes\n", mAllocatedMem);
	printf("  recycled: %u small buff with %u allocated chunks and %u buff returned to the pool\n", mSmallRecycleCount, mSmallBufferCount, mSmallPool.GetSize());
	printf("  recycled: %u medium buff with %u allocated chunks and %u buff returned to the pool\n", mMediumRecycleCount, mMediumBufferCount, mMediumPool.GetSize());
	printf("  recycled: %u large buff with %u allocated chunks and %u buff returned to the pool\n", mLargeRecycleCount, mLargeBufferCount, mBigPool.GetSize());
}

void BufferPool::init()
{
	printf("buffer pool mem init start\n");
	/* fill 0x100 buffer queue */
	for (int i = 0; i < 2000; i++)
	{
		void * queueEntry = ASCENT_MALLOC(0x100);
		mSmallPool.Push(queueEntry);
		mAllocatedMem+=0x100;
		mSmallBufferCount++;
	}
	
	/* fill 0x10000 buffer queue */
	for (int i = 0; i < 50; i++)
	{
		void * queueEntry = ASCENT_MALLOC(0x1000);
		mMediumPool.Push(queueEntry);
		mAllocatedMem+=0x1000;
		mMediumBufferCount++;
	}

	/* fill 0x10000 buffer queue */
	for (int i = 0; i < 10; i++)
	{
		void * queueEntry = ASCENT_MALLOC(0x100000);
		mBigPool.Push(queueEntry);
		mAllocatedMem+=0x100000;
		mLargeBufferCount++;
	}

	printf("buffer pool mem init end\n");
}

void BufferPool::ResizeBuffer( void** buffer, size_t oldsize, size_t newsize, bool /*copy*/ /*= false*/)
{
	//assert(buffer);
	//assert(*buffer);
	//if (copy == true)
	//{
	//	void * newbuff = GetBuffer(newsize);
	//	ASCENT_MEMCPY(newbuff, *buffer, oldsize);
	//	DumpBuffer(*buffer, oldsize);
	//}
	//else
	//{
		DumpBuffer(*buffer, oldsize);
		*buffer = GetBuffer(newsize);
	//}
}