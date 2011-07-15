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
#include "ascent.h"
#include "RleModule.h"

bool RleModule::decode( unsigned char* src, const size_t src_len, unsigned char* dst, size_t* dst_len )
{
	bool has_second_part;
	unsigned int read_index;
	unsigned int write_index;
	int rle_opcode;
	int chunkSize;
	int blockSize;
	bool temp_has_second_part;

	size_t dst_size = *dst_len;

	has_second_part = false;
	write_index = 0;
	read_index = 0;

	if ( src_len )
	{
		while ( write_index < dst_size )
		{
			if ( has_second_part == true )
			{
				rle_opcode = rle_opcode >> 4;
			}
			else
			{
				rle_opcode = src[read_index++];
			}

			chunkSize = (rle_opcode & 0xF) - 8;
			temp_has_second_part = has_second_part == false;
			if ( chunkSize < 0 )
			{
				if ( write_index - chunkSize > dst_size )
				{
					Log.Error("RleModule", "Invalid RLE string");

					// this needs some love...
					ASCENT_HARDWARE_BREAKPOINT;

					return false;
				}

				while ( 1 )
				{
					++chunkSize;
					if ( read_index >= src_len )
						goto LABEL_2;
					dst[write_index++] = src[read_index++];
					if ( !chunkSize )
						goto LABEL_17;
				}
			}

			if ( chunkSize + write_index + 1 > dst_size )
			{
				Log.Error("RleModule", "Invalid RLE string");

				// this needs some love...
				ASCENT_HARDWARE_BREAKPOINT;

				return false;
			}

			blockSize = chunkSize + 1;
			memset(&dst[write_index], 0, blockSize);
			write_index += blockSize;
LABEL_17:
			if ( read_index >= src_len )
				goto LABEL_2;
			has_second_part = temp_has_second_part;
		}
	}
	else
	{
LABEL_2:
		if ( write_index < dst_size )
			memset(&dst[write_index], 0, dst_size - write_index);
	}

	/* I think this is correct.... but I actually don't know hehe */
	*dst_len = write_index;

	return true;
}
