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

#ifndef _PYUNICODEUCS2_H
#define _PYUNICODEUCS2_H

#ifdef ENABLE_PACKED_CLASSES
#  pragma pack(push,1)
#endif

class PyUnicodeUCS2 : public PyObject
{
public:
	PyUnicodeUCS2();
	PyUnicodeUCS2(const wchar_t* str);
	PyUnicodeUCS2(const wchar_t* str, const size_t len);
	PyUnicodeUCS2(const std::wstring& str);
	~PyUnicodeUCS2();
    uint32 hash();

	bool set(const wchar_t* str, size_t len);
	bool resize(size_t newsize);
	wchar_t * content() const;
	size_t size() const;

private:
	wchar_t* mStr;
	size_t mStrLen;
	uint32 mHashValue; /* string hash cache */
};


#ifdef ENABLE_PACKED_CLASSES
#  pragma pack(pop)
#endif

/************************************************************************/
/* fake python api                                                      */
/************************************************************************/

/**
 * @brief 
 *
 * 
 *
 * @param[in]
 * @param[out]
 * @return
 */
static PyUnicodeUCS2* PyUnicodeUCS2_FromWideChar(const wchar_t* str, size_t len)
{
	PyUnicodeUCS2 * string = new PyUnicodeUCS2(str, len);
	return string;
}

/**
 * @brief Python API cloning, converting a UTF8 string to a unicode UCS2 string.
 *
 * 
 *
 * @note this isn't as close to the native python implementation... we will see how far this will get us.
 * @param[in] str is the const char string that is fed into the function.
 * @param[in] len is the length of the char string that is converted by this function.
 * @return a freshly generated PyUnicodeUCS2 object from the char string
 * @note yea I know this function lacks 1 parameter, which is "const char *errors"
 */
static PyUnicodeUCS2* PyUnicodeUCS2_DecodeUTF8(const char* str, size_t len)
{
	PyUnicodeUCS2 * result = new PyUnicodeUCS2();

    if (!result->resize(len)) {
        PyDecRef(result);
        //lint -e{429} suppress "Custodial pointer has not been freed or returned"
        return NULL;
    }

	size_t newSize = mbstowcs(result->content(), str, len);
	result->content()[len] = '\0';
	
	if (newSize != len) {
		PyDecRef(result);
        //lint -e{429} suppress "Custodial pointer has not been freed or returned"
		return NULL;
	}

	return result;
}

static PyObject *PyUnicode_AsUTF8String(PyObject *unicode)
{
	if (unicode == NULL || !PyUnicode_Check(unicode))
		return NULL;

	PyUnicodeUCS2 * str = (PyUnicodeUCS2 *)unicode;

	PyString * res = new PyString();
    if (!res->resize(str->size())) {
        PyDecRef(res);
        //lint -e{429} suppress "Custodial pointer has not been freed or returned"
        return NULL;
    }
		
	size_t ret_len = wcstombs((char*)&res->content()[0], str->content(), str->size());

	if (ret_len != str->size()) {
		PyDecRef(res);
        //lint -e{429} suppress "Custodial pointer has not been freed or returned"
		return NULL;
	}

	return (PyObject *)res;
}

#endif // _PYUNICODEUCS2_H