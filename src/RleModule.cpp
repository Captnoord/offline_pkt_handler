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
