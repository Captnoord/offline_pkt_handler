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
#include "WriteStream.h"

WriteStream::WriteStream() : mWriteIndex(0), mSize(WS_DEFAULT_BUFF_SIZE), mBuffer(static_cast<uint8*>(ASCENT_MALLOC(mSize)))
{
	ASCENT_ASSERT(mBuffer);
}

WriteStream::WriteStream( const size_t len ) : mWriteIndex(0), mSize(len), mBuffer(static_cast<uint8*>(ASCENT_MALLOC(mSize)))
{
	ASCENT_ASSERT(mBuffer);
}

WriteStream::~WriteStream()
{
	SafeFree(mBuffer);
}

bool WriteStream::writeOpcode( uint8 opcode )
{
	if (!reserve(1))
        return false;

	mBuffer[mWriteIndex++] = opcode;
	return true;
}

bool WriteStream::writeSizeEx( size_t size_ex )
{
    if ( !reserve(1) )
        return false;

	if (size_ex > 0xFE)
	{
		mBuffer[mWriteIndex++] = uint8(-1);
		if ( reserve(4) )
		{
			*((uint32*)(&mBuffer[mWriteIndex])) = (uint32)size_ex;
			mWriteIndex+=4;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		mBuffer[mWriteIndex++] = (uint8)size_ex;
		return true;
	}
}

const size_t WriteStream::tell() const
{
	return mWriteIndex;
}

bool WriteStream::seek( int32 offset, int origin )
{
	switch (origin)
	{
	case SEEK_SET:
		{
			// (signed) maybe break the stuff...
			if (offset <= (signed)mSize)
				mWriteIndex = offset;
			else
				return false;
		} return true;
	case SEEK_CUR:
		{
			if (mWriteIndex + offset <= mSize)
				mWriteIndex += offset;
			else
				return false;
		} return true;
	case SEEK_END:
		{
			if (int32(mSize) - offset >= 0)
				mWriteIndex = mSize - offset;
			else
				return false;
		} return true;
	default:
        return false;
	}
}

bool WriteStream::write( const char* buffer, const size_t len )
{
	if (buffer == NULL || len == 0)
		return false;

	if (!reserve(len))
		return false;
	
	void* dest = static_cast<void*>(&mBuffer[mWriteIndex]);
	ASCENT_MEMCPY(dest, buffer, len);
	mWriteIndex+=len;
	return true;
}

bool WriteStream::reserve( const size_t reserve_len )
{
	/* if the current allocated amount of memory is enough */
	if (mSize > mWriteIndex + reserve_len)
		return true;

	/* @todo plan nice memory tricks here */
	mSize = mWriteIndex + reserve_len;
	mBuffer = static_cast<uint8*>(ASCENT_REALLOC(mBuffer, mSize));
	if (mBuffer == NULL)
		return false;
	return true;
}

bool WriteStream::insert( const uint8* buff, size_t buff_len, size_t index )
{
    size_t data_size = size();

    /* check if the index is bigger than the size of the data */
    if (index > data_size)
        return false;

    /* the amount of data that needs to be moved */
    const int delta_size = (int)data_size - (int)index;

    /* shouldn't be able to move negative count of data */
    if (delta_size < 0)
        return false;

    /* make sure we have enough room for the insert */
    if(reserve(buff_len) == false)
        return false;

    void* begin = static_cast<void*>(&mBuffer[index]);
    void* delta = static_cast<void*>(&mBuffer[index + buff_len]);

    /* check if the source and destination is valid */
    if (begin == NULL)
        return false;

    if (delta == NULL)
        return false;

    /* evil and dangerous part
     * note: memcpy over memmove maybe faster
     */
    memmove(delta, begin, delta_size);
    memcpy(begin, buff, buff_len);

    /* evil shit */
    mWriteIndex+=buff_len;
    return true;
}

uint8* WriteStream::content()
{
	return mBuffer;
}

size_t WriteStream::size()
{
	return mWriteIndex;
}

size_t WriteStream::allocatedsize() const
{
	return mSize;
}
