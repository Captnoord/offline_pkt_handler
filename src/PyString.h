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

#ifndef PYSTRING_H
#define PYSTRING_H

#ifdef ENABLE_PACKED_CLASSES
#  pragma pack(push,1)
#endif

class PyString : public PyObject
{
public:
	PyString();
	PyString(const char* str);
	PyString(const char* str, size_t len);
	PyString(std::string& str);
	~PyString();
    uint32 hash();
	bool set(const char* str, size_t len);
	const char* content();
	const size_t length();

	/**
	 * \brief appends a string to the end of the PyString
	 *
	 * @param[in] str the std string that should be appended to the PyString.
	 * @return this function should return the "this" object.
	 */
	PyString& append ( const std::string& str );

	/**
	 * \brief appends a string to the end of the PyString
	 *
	 * @param[in] str the std string that should be appended to the PyString.
	 * @param[in] pos where the string should be appended.
	 * @param[in] n how much of the string should be appended.
	 * @return this function should return the "this" object.
	 */
	PyString& append ( const std::string& str, size_t pos, size_t n );

	bool resize(size_t size);

	bool operator==(const char *str1);
	PyString &operator=(const char *str);

    /**
     * @note we need this operator?
     */
	char &operator[](const int index);
private:

	char* mStr;
	size_t mStrLen;
	uint32 mHashValue;
};

#ifdef ENABLE_PACKED_CLASSES
#  pragma pack(pop)
#endif

/**
 * wrapper function to create a PyString from a buffer
 *
 * @param[in] str the const char buffer that is fed into the constructor of the object.
 * @param[in] len the length of the buffer.
 * @return this function should return the created PyString and NULL if it failed.
 */
PyString* PyString_FromStringAndSize(const char* str, size_t len);

#endif // PYSTRING_H