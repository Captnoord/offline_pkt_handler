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
    Original work by: Entity
*/

/*
// dbrow.c - DBRow decoder
//
// Copyright (c) 2003-2011 Jamie "Entity" van den Berge <jamie@hlekkir.com>
//
// This code is free software; you can redistribute it and/or modify
// it under the terms of the BSD license (see the file LICENSE.txt
// included with the distribution).
*/

#ifndef _RLEMODULE_H
#define _RLEMODULE_H

/* RLE String module, contains both compress and uncompress routines */

class RleModule
{
public:

    static void pack( const char *in, int in_size, char *out, int *out_size );

	/**
	 * \brief unpack decodes the Zero 'Rle' data.
	 *
	 * @param[in] src the source buffer.
 	 * @param[in] src_len the length of the source buffer.
	 * @param[out] dst the destination buffer, needs to be large enough.
	 * @param[out] dst_len the length of the destination buffer.
	 * @return true if decoded correctly and false if a error has happened.
	 */
	static bool unpack( const unsigned char* src, const int src_len, unsigned char* dst, const int* dst_len );

};
#endif // _RLEMODULE_H
