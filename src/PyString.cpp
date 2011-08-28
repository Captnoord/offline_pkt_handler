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
#include "Python.h"

/************************************************************************/
/* PyString code                                                        */
/************************************************************************/
PyString::PyString() : PyObject(PyTypeString), mStr(NULL), mStrLen(0), mHashValue(0) {}

PyString::PyString(const char* str) : PyObject(PyTypeString), mStr(NULL), mStrLen(0), mHashValue(0)
{
	size_t len = strlen(str);
	(void)set(str, len);
}

PyString::PyString(const char* str, size_t len) : PyObject(PyTypeString), mStr(NULL), mStrLen(0), mHashValue(0)
{
	(void)set(str, len);
}

PyString::PyString(std::string& str) : PyObject(PyTypeString), mStr(NULL),  mStrLen(0), mHashValue(0)
{
	(void)set(str.c_str(), str.size());
}

PyString::~PyString()
{
	SafeFree(mStr);

	mStrLen = 0;
}

bool PyString::set(const char* str, size_t len)
{
	if (str == NULL || len == 0)
		return false;

	if (mStr != NULL)
		mStr = static_cast<char*>(ASCENT_REALLOC(mStr, len+1));
	else
		mStr = static_cast<char*>(ASCENT_MALLOC(len+1));

	mStrLen = len;
	
	if (mStr == NULL)
		return false;

	ASCENT_MEMCPY(mStr, str, mStrLen);
	mStr[mStrLen] = '\0';
	return true;
}

const char* PyString::content()
{
	return mStr;
}

const size_t PyString::length()
{
	return mStrLen;
}

/* atm only borrowing from the slightly bloated append function.. */
PyString& PyString::append( const std::string& str )
{
	return append(str, mStrLen, str.size());
}

/* the trick is that we can also append a string to a empty string. */
PyString& PyString::append( const std::string& str, size_t pos, size_t n )
{
	// we are appending a empty string
	if(mStrLen == 0)
	{
		if (n > str.size())
			n = str.size();

		// we can use the set function as we are empty
		(void)set(str.c_str(), n);
		return *this;
	}
	else // we aren't a empty string... complicated
	{
		size_t newSize = mStrLen + n;
		assert(mStr); // we should have a existing piece of memory

		mStr = (char*)ASCENT_REALLOC(mStr, newSize + 1); // realloc... it may move memory around...

        /* this would mean a huge fail */
        if (mStr == NULL)
            return *this;
		
		// the point where its added
		char* crossA = reinterpret_cast<char*>(&mStr[pos]);

		// the point where it sends
		char* crossB = reinterpret_cast<char*>(&mStr[pos + n]);

		// chunk new_size
		size_t crossSize = mStrLen - pos;

		// first move that chunk
		memcpy(crossB,crossA, crossSize);

		// append the string
		memcpy(crossA,str.c_str(), n);

		mStrLen = newSize;
		mStr[mStrLen] = '\0';
		return *this;
	}
}

bool PyString::resize( size_t new_size )
{
	/* handling rare conditions separately */
	if (mStr != NULL && new_size == 0)
	{
		SafeFree(mStr);
		mStrLen = new_size;
		return true;
	}

	mStrLen = new_size;

	if (mStr != NULL)
		mStr = static_cast<char*>(ASCENT_REALLOC(mStr, mStrLen+1));
	else
		mStr = static_cast<char*>(ASCENT_MALLOC(mStrLen+1));

	if (mStr == NULL)
		return false;

	mStr[mStrLen] = '\0';
	return true;

}

bool PyString::operator==( const char *str1 )
{
    if (str1 == NULL || mStr == NULL)
        return false;

	return (strncmp(mStr, str1, mStrLen) == 0);
}

PyString & PyString::operator=( const char *str )
{
	size_t len = strlen(str);
	if (!set(str, len))
        sLog.Error("PyString", "unable to set pystring");
	return *this;
}

uint32 PyString::hash()
{
    if (mHashValue == 0)
    {
        mHashValue = Utils::Hash::sdbm_hash(mStr, (int)mStrLen);
        return mHashValue;

    }
    return mHashValue;
}

PyString* PyString_FromStringAndSize(const char* str, size_t len)
{
    PyString * string = new PyString(str, len);
    return string;
}
