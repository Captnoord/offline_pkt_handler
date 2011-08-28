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

bool RleModule::unpack( const unsigned char* in, const int in_size, unsigned char* out, const int* out_size )
{
    int in_ix = 0;
	int out_ix = 0;
	int count;
	int run = 0;
	int nibble = 0;

	while(in_ix < in_size)
	{
		nibble = !nibble;
		if(nibble)
		{
			run = (unsigned char)in[in_ix++];
			count = (run & 0x0f) - 8;
		}
		else
			count = (run >> 4) - 8;

		if(count >= 0)
		{
			if (out_ix + count + 1 > *out_size)
				return false;

			while(count-- >= 0)
				out[out_ix++] = 0;
		}
		else
		{
			if (out_ix - count > *out_size)
				return false;

			while(count++ && in_ix < in_size)
				out[out_ix++] = in[in_ix++];
		}
	}

	while(out_ix < *out_size)
		out[out_ix++] = 0;

	return true;
}

void RleModule::pack( const char *in, int in_size, char *out, int *out_size )
{
    int nibble = 0;
    int nibble_ix = 0;
    int in_ix = 0;
    int out_ix = 0;
    int start, end, count;
    int zerochains = 0;

    while(in_ix < in_size)
    {
        if(!nibble)
        {
            nibble_ix = out_ix++;
            out[nibble_ix] = 0;
        }

        start = in_ix;
        end = in_ix+8;
        if(end > in_size)
            end = in_size;

        if(in[in_ix])
        {
            zerochains = 0;
            do {
                out[out_ix++] = in[in_ix++];
            } while(in_ix<end && in[in_ix]);
            count = (start - in_ix) + 8;
        }
        else
        {
            zerochains++;
            while(in_ix<end && !in[in_ix])
                in_ix++;
            count = (in_ix - start) + 7;
        }

        if(nibble)
            out[nibble_ix] |= (count << 4);
        else
            out[nibble_ix] = count;
        nibble = !nibble;
    }

    if(nibble && zerochains)
        zerochains++;

    while(zerochains>1)
    {
        zerochains -= 2;
        out_ix -= 1;
    }

    *out_size = out_ix;
}
