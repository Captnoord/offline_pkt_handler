#ifndef _FILE_PARSER_H
#define _FILE_PARSER_H

#include <stdio.h>

FILE* fp_in;
FILE* fp_out;

uint8 Getuint8()
{
	uint8 buff;
	fread(&buff,1,1,fp_in);
	return buff;
}

uint16 Getuint16()
{
	uint16 buff;
	fread(&buff,2,1,fp_in);
	return buff;
}

uint32 Getuint32()
{
	uint32 buff;
	fread(&buff,4,1,fp_in);
	return buff;
}

int32 Getint32()
{
	int32 buff;
	fread(&buff,4,1,fp_in);
	return buff;
}

uint64 Getuint64()
{
	uint64 buff;
	fread(&buff,8,1,fp_in);
	return buff;
}

char* GetBuffer(size_t len)
{
	char* buff = (char*)ASCENT_MALLOC(len);
	assert(buff);
	fread(buff, len,1,fp_in);
	return buff;
}

void ParseFile(const char* in_file_path, const char* out_file_path)
{
	std::string out_path;
	if (out_file_path == NULL)
	{
		out_path = in_file_path;
		out_path.resize(out_path.size() - 4);
		out_path+="_parsed.txt";
	}
	else
	{
		out_path = out_file_path;
	}

	fp_in = fopen(in_file_path,"rb");
	fp_out = fopen(out_path.c_str(), "w");

	long fsize = 0;
	fseek(fp_in,0,SEEK_END);
	fsize = ftell(fp_in);
	fseek(fp_in,0,SEEK_SET);

	size_t i = 0;
	while (ftell(fp_in) < fsize)
	{
		uint32 FileOffset = ftell(fp_in);
		time_t timestamp = Getuint64();
		uint8 direction = Getuint8();
		int length = Getint32();
		char* packetBuf = GetBuffer(length);

		fprintf(fp_out, "\n{%s} FileOffset:0x%X, Packet Nr:%u, length:%u, time:%u\n", (direction ? "SERVER" : "CLIENT"), FileOffset, i, length, uint32(timestamp & 0xFFFFFFFF));
		
		ReadStream readstream(packetBuf, length);

		MarshalStream stream;
		PyObject* henk = stream.load(readstream);

		if (henk != NULL)
		{
#ifdef MARSHAL_REMARSHAL

			WriteStream writeStream(length);
			bool saveret = stream.save(henk, writeStream);

			if (saveret == false)
			{
				HexAsciiModule::print_hexview(stdout, (char*)packetBuf, length);
				DumpObject(stdout, henk);
				HexAsciiModule::print_hexview(stdout, (char*)writeStream.content(), writeStream.size());
				ASCENT_HARDWARE_BREAKPOINT;
			}

			size_t writeStreamSize = writeStream.size();

			//debug checking exception, client packets had a hash in front of them I am not including in the marshal write 
			if (direction == 0)
			{
				if (readstream.buffersize() != writeStream.size())
					writeStream.insert((uint8*)"\x1C\0\0\0\0", 5, 5);
			}
			else
			{
				// do the mem compare to check if the rebuild packets are the same 
				if(memcmp(writeStream.content(), readstream.content(), readstream.size()))
				{
					DumpObject(stdout, henk);
					HexAsciiModule::print_hexview(stdout, (char*)packetBuf, length);
					HexAsciiModule::print_hexview(stdout, (char*)writeStream.content(), writeStream.size());
					//ASCENT_HARDWARE_BREAKPOINT;
				}
			}

			if (readstream.buffersize() != writeStream.size())
			{
				DumpObject(stdout, henk);
				HexAsciiModule::print_hexview(stdout, (char*)packetBuf, length);
				HexAsciiModule::print_hexview(stdout, (char*)writeStream.content(), writeStream.size());
				//ASCENT_HARDWARE_BREAKPOINT;
			}
#endif//ENABLE_MARSHAL_SAVE_CHECK

			DumpObject(fp_out, henk);
			henk->DecRef();
		}
		
		ASCENT_FREE(packetBuf);
		
		if (readstream.tell()!= readstream.size())
			Log.Warning("fileParser","sub stream isn't parsed completely: %u - %u", readstream.tell(), readstream.size());

		//printf("Parsed packet nr: %u | offset: %u, size: %u\n", i, readstream.tell(), readstream.size());

		//if (i == 5000)
		//	break;
		i++;
	}

	fclose(fp_out);
	fclose (fp_in);

	printf("\nParsing done: parsed:%u packets\n", i);
}

/* method prepping for parsing cache files */
void ParseCacheFile(const char* in_file_path, const char* out_file_path)
{
	std::string out_path;
	if (out_file_path == NULL)
	{
		out_path = in_file_path;
		//out_path.resize(out_path.size() - 4);
		out_path+="_parsed.txt";
	}
	else
	{
		out_path = out_file_path;
	}

	fp_in = fopen(in_file_path,"rb");
	fp_out = fopen(out_path.c_str(), "w");

	long fsize = 0;
	fseek(fp_in,0,SEEK_END);
	fsize = ftell(fp_in);
	fseek(fp_in,0,SEEK_SET);

	/* we read the entire file because its 1 marshal file/object/packet
	 * @note we assume this, this obviously doesn't mean its correct.
	 */
	char* packetBuf = GetBuffer(fsize);

	ReadStream readstream(packetBuf, fsize);

	MarshalStream stream;
	PyObject* henk = stream.load(readstream);

	if (henk != NULL)
	{
		DumpObject(fp_out, henk);
		henk->DecRef();
	}

	fflush(fp_out);

	/* check if we where capable of parsing the entire packet, if not warn us about it */
	if (readstream.tell()!= readstream.size())
		Log.Warning("fileParser","sub stream isn't parsed completely: %u - %u", readstream.tell(), readstream.size());

	printf("Parsed cache file | offset: %u, size: %u\n", readstream.tell(), readstream.size());
	
	SafeDeleteArray(packetBuf);
	fclose(fp_out);
}

#endif//_FILE_PARSER_H
