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

#ifndef _RLEMODULE_H
#define _RLEMODULE_H

/* RLE String module, contains both compress and uncompress routines */

class RleModule
{
public:

	static void PackZeroCompressed(const uint8 *src, uint32 len, std::vector<uint8> &out_buf, size_t &dst_len)
	{
		/* get the end pointer */
		const uint8 *end = &src[len];
		uint32 write_index = 0;

		while(src < end)
		{
			// we need to have enough room without moving (otherwise
			// it would invalidate our pointer obtained below); size
			// is 1 byte of opcode + at most 2x 8 bytes
			out_buf.resize(out_buf.size() + 1 + 16);

			// insert opcode
			out_buf[write_index] = 0;								// insert opcode placeholder
			ZCOpCode *opcode = (ZCOpCode *)&out_buf[write_index++];	// obtain pointer to it

			// encode first part
			if(*src == 0x00)
			{
				//we are starting with zero, hunting for non-zero
				opcode->f_isZero = true;
				opcode->f_len = 0;

				src++;
				for(; src < end && *src == 0x00 && opcode->f_len < 7; opcode->f_len++)
					src++;
			}
			else
			{
				//we are starting with data, hunting for zero
				opcode->f_isZero = false;
				opcode->f_len = 7;

				out_buf[write_index++] = *src++;
				for(; src < end && *src != 0x00 && opcode->f_len > 0; opcode->f_len--)
					out_buf[write_index++] = *src++;
			}

			//if (write_index == len)
			//	break;

			//if ()

			if(src >= end)
				break;

			// encode second part
			if(*src == 0x00) {
				//we are starting with zero, hunting for non-zero
				opcode->s_isZero = true;
				opcode->s_len = 0;

				src++;
				for(; src < end && *src == 0x00 && opcode->s_len < 7; opcode->s_len++)
					src++;
			} else {
				//we are starting with data, hunting for zero
				opcode->s_isZero = false;
				opcode->s_len = 7;

				out_buf[write_index++] = *src++;
				for(; src < end && *src != 0x00 && opcode->s_len > 0; opcode->s_len--)
					out_buf[write_index++] = *src++;
			}			
		}
		dst_len = write_index;

		//_log(NET__ZEROCOMP,
		//	"  Zero-compressing buffer resulted in %lu bytes (ratio %.02f)",
		//	out_buf.size(), double(out_buf.size()) / double(len)
		//	);
	}

	/**
	 * \brief decode decodes the Zero 'Rle' data.
	 *
	 * @param[in] src the source buffer.
 	 * @param[in] src_len the length of the source buffer.
	 * @param[out] dst the destination buffer, needs to be large enough.
	 * @param[out] dst_len the length of the destination buffer.
	 * @return true if decoded correctly and false if a error has happened.
	 * @todo clean this function up, so it doesn't look as ugly as it does now.
	 * @note as my teacher always screamed at me "DON'T USE GOTO'S AND LABELS".
	 */
	static bool decode(unsigned char* src, const size_t src_len, unsigned char* dst, size_t* dst_len);

	static char zerounpackex(char *dst, unsigned int dst_len, char *src, unsigned int src_len)
	{
		bool hasSecondPart; // edx@1
		char *t_dst; // ebp@1
		unsigned int read_index; // edi@1
		unsigned int write_index; // esi@1
		char *t_src; // ecx@7
		signed int tempChunk; // eax@8
		int tempChunkSize; // eax@10
		unsigned int src_len_t; // edx@12
		int v13; // ebx@12
		bool hasSecondPart_t; // [sp+14h] [bp-4h]@5
		signed int chunkPart; // [sp+10h] [bp-8h]@8

		t_dst = dst;
		hasSecondPart = 0;
		write_index = 0;
		read_index = 0;
		if ( src_len )
		{
			while ( write_index < dst_len )
			{
				t_src = src;
				if ( hasSecondPart )
				{
					tempChunk = chunkPart >> 4;
				}
				else
				{
					tempChunk = src[read_index];
					tempChunk = tempChunk & 0xFF;

					chunkPart = src[read_index++];
				}
				tempChunkSize = (tempChunk & 0xF) - 8;
				hasSecondPart_t = hasSecondPart == 0;
				if ( tempChunkSize < 0 )
				{
					if ( write_index - tempChunkSize > dst_len )
					{
LABEL_19:
						//PyErr_SetString(PyExc_RuntimeError, "Invalid RLE string");
						ASCENT_HARDWARE_BREAKPOINT;
						return 0;
					}
					src_len_t = src_len;
					while ( 1 )
					{
						++tempChunkSize;
						if ( read_index >= src_len_t )
							goto LABEL_2;
						t_dst[write_index++] = t_src[read_index++];
						if ( !tempChunkSize )
							goto LABEL_17;
					}
				}
				if ( tempChunkSize + write_index + 1 > dst_len )
					goto LABEL_19;
				v13 = tempChunkSize + 1;
				memset(&t_dst[write_index], 0, tempChunkSize + 1);
				src_len_t = src_len;
				write_index += v13;
LABEL_17:
				if ( read_index >= src_len_t )
					goto LABEL_2;
				hasSecondPart = hasSecondPart_t;
			}
		}
		else
		{
LABEL_2:
			if ( write_index < dst_len )
				memset(&t_dst[write_index], 0, dst_len - write_index);
		}
		return 1;
	}

	
#pragma pack(push,1)
	// ZeroCompress OpCode
	struct ZCOpCode {
		// first part
		uint8 f_len : 3;
		bool f_isZero : 1;

		// second part
		uint8 s_len : 3;
		bool s_isZero : 1;
	};
#pragma pack(pop)

#define _log(x,...) //

	typedef std::vector<uint8> zeropackbuff;
	// for verify purposes
	static void UnpackZeroCompressed(const uint8 *in_buf, uint32 in_length, zeropackbuff &buffer) {
		buffer.clear();
		if(in_buf == NULL || in_length == 0)
			return;

		_log(NET__ZEROINFL, "Zero-inflating buffer of length %d", in_length );

		ZCOpCode opcode;
		const uint8 *end = in_buf + in_length;

		while(in_buf < end) {
			opcode = *(ZCOpCode *)(in_buf++);

			if(opcode.f_isZero) {
				uint8 count = opcode.f_len+1;
				_log(NET__ZEROINFL, "    Opcode 0x%x (first part) yields %d zero bytes at %d", opcode, count, buffer.size());
				for(; count > 0; count--) {
					buffer.push_back(0);
				}
			} else {
				uint8 count = 8-opcode.f_len;
				_log(NET__ZEROINFL, "    Opcode 0x%x (first part) yields %d data bytes at %d", opcode, count, buffer.size());
				//caution: if we are at the end of the buffer, its possible to be processing a '0' opcode, which should mean "8 bytes", but really means "end"
				for(; count > 0 && in_buf < end; count--) {
					buffer.push_back(*in_buf++);
				}
			}

			if(opcode.s_isZero) {
				uint8 count = opcode.s_len+1;
				_log(NET__ZEROINFL, "    Opcode 0x%x (second part) yields %d zero bytes at %d", opcode, count, buffer.size());
				for(; count > 0; count--) {
					buffer.push_back(0);
				}
			} else {
				uint8 count = 8-opcode.s_len;
				_log(NET__ZEROINFL, "    Opcode 0x%x (second part) yields %d data bytes at %d", opcode, count, buffer.size());
				//caution: if we are at the end of the buffer, its possible to be processing a '0' opcode, which should mean "8 bytes", but really means "end"
				for(; count > 0 && in_buf < end; count--) {
					buffer.push_back(*in_buf++);
				}
			}
		}

		_log(NET__ZEROINFL, "  Zero-inflating buffer yields %d inflated bytes (ratio %.02f)", buffer.size(), double(in_length) / double(buffer.size()));
	}
};
#endif // _RLEMODULE_H