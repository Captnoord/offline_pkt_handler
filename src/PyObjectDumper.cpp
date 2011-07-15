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
#include "zlib.h"

DbgHeap g_Heap;

void DumpObject(FILE * fd, PyObject* obj)
{
	Dump(fd, obj, 0);
	fflush(fd);
}

#define DUMP_ITR_SPACE 2

void Dump( FILE * fp, PyObject * obj, size_t deep, bool isItr /*= false*/, bool convertNull /*= false*/ )
{
	if (obj == NULL)
	{
		printf("Can not dump NULL object\n");
		return;
	}
	//printf("Deep: %d, PyObject type: 0x%X, filtered: 0x%X\n", deep, obj->gettype(), obj->gettype());

	/* this one makes sure that we have slightly formatted output (important for readability) */
	if(isItr == false && deep != -1)
		WriteSpacer(fp, deep);

	switch(obj->gettype())
	{
	case PyTypeNone:
		if (convertNull == false)
			fputs("PyNone", fp);
		else
			fputs("NULL", fp);

		if (deep != -1)
			fputc('\n', fp);
		break;

	case PyTypeBool:
		{
			PyBool & Bool = *((PyBool*)obj);

			if (isItr == false)
				fputs("PyBool:", fp);

			if (Bool == true)
				fputs("true", fp);
			else
				fputs("false", fp);

			/* part of a hack to handle iterators of class objects */
			if (deep != -1)
				fputc('\n', fp);
		}
		break;

	case PyTypeInt:
		{
			PyInt & num = *((PyInt*)obj);

			if (isItr == false)
				fputs("PyInt:", fp);

			int32 val = num.GetValue();
			if ( val < 10 && val > -10 || deep == -1)
				fprintf(fp, "%d", num.GetValue());
			else
				fprintf(fp, "%d    <0x%X>", num.GetValue(), num.GetValue());

			/* part of a hack to handle iterators of class objects */
			if (deep != -1)
				fputc('\n', fp);
		}
		break;

	case PyTypeLong:
		{
			PyLong & num = *((PyLong*)obj);

			if (isItr == false)
				fputs("PyLong:", fp);

			int64 val = num.GetValue();
			if ( val < 10 && val > -10 || deep == -1)
				fprintf(fp, I64FMTD, num.GetValue());
			else
				fprintf(fp, I64FMTD"    <0x%I64X>", num.GetValue(), num.GetValue());

			/* part of a hack to handle iterators of class objects */
			if (deep != -1)
				fputc('\n', fp);
		}
		break;

	case PyTypeReal:
		{
			PyFloat * num = (PyFloat*) obj;
			if (isItr == false)
				fputs("PyFloat:", fp);

			fprintf(fp, "%f", num->GetValue());

			/* part of a hack to handle iterators of class objects */
			if (deep != -1)
				fputc('\n', fp);
		}
		break;

	case PyTypeString:
		{
			PyString * str = (PyString*) obj;

			if (isItr == false)
				fputs("PyString:", fp);

			/* check if the string is readable, atm we are using the percentage of readable characters and consider 80% the limit
			 * this also is a strain on the CPU... so use it carefully
			 */
			if (IsPrintString(str) == false)
			{
				// check if its a buffered marshal sub stream
				if (str->content() != NULL && (str->content()[0] == 0x78 || str->content()[0] == '~' || str->content()[0] == 0x63 || str->content()[0] == 0xB3))
				{
					ReadStream *rawsubstream = new ReadStream((const char*)str->content(), str->length());
					MarshalStream *substream = new MarshalStream();
					PyObject* obj1 = substream->load(*rawsubstream);
					if (obj1 != NULL)
					{
#ifdef MARSHAL_REMARSHAL
						WriteStream writeStream(rawsubstream->size());
						bool saveret = substream->save(obj1, writeStream);

						if (saveret == false)
						{
							DumpObject(stdout, obj1);
							HexAsciiModule::print_hexview(stdout, (char*)rawsubstream->content(), rawsubstream->size());
							HexAsciiModule::print_hexview(stdout, (char*)writeStream.content(), writeStream.size());
							ASCENT_HARDWARE_BREAKPOINT;
						}

						if(memcmp(writeStream.content(), rawsubstream->content(), rawsubstream->size()))
						{
							DumpObject(stdout, obj1);
							HexAsciiModule::print_hexview(stdout, (char*)rawsubstream->content(), rawsubstream->size());
							HexAsciiModule::print_hexview(stdout, (char*)writeStream.content(), writeStream.size());
							ASCENT_HARDWARE_BREAKPOINT;
						}
#endif//MARSHAL_REMARSHAL

						Dump(fp, obj1, deep+4);
						obj1->DecRef();
					}
					else
					{
						fputc('\n', fp);
					}

					/* generate a warning for incomplete parsed substreams its because we want to know if we didn't parse some stuff */
					if (rawsubstream->tell()!= rawsubstream->size())
						Log.Warning("PyDumper","sub stream isn't parsed completely: %u - %u", rawsubstream->tell(), rawsubstream->size());

					SafeDelete(substream);
					SafeDelete(rawsubstream);
				}
				else
				{
					/* handle buffers that aren't marshal streams but don't consist of enough humanly readable characters */
					fputc('\"',fp);
					HexAsciiModule::hexlify(fp, str->content(), str->length());
					fputc('\"',fp);				

					/* part of a hack to handle iterators of class objects */
					if (deep != -1)
						fputc('\n', fp);
				}
			}
			else
			{
				/* the PyString consists of only readable characters ( at least 80% of it ) so we can print it as normal text */
				if (str->length() == 0 || str->content() == NULL)
					fputs("\"\"", fp);
				else
					fprintf(fp, "\"%s\"", str->content());

				/* part of a hack to handle iterators of class objects */
				if (deep != -1)
					fputc('\n', fp);
			}
		}
		break;

	case PyTypeUnicode:
		{
			/* atm we don't handle special unicode cases but I think there are some.... */
			PyUnicodeUCS2 * str = (PyUnicodeUCS2*) obj;
			if (isItr == false)
				fputs("PyUnicode:", fp);

			if (str->length() == 0 || str->content() == NULL)
				fputs("\"\"", fp);
			else
				fprintf(fp, "\"%S\"", str->content());

			/* part of a hack to handle iterators of class objects */
			if (deep != -1)
				fputc('\n', fp);
		}
		break;

	case PyTypeDict:
		{
			/* if our dict is a itr, we need to read it as a part of a tuple/string/dict. So we need to add 2 spaces extra */
			if (isItr == true)
				deep+=DUMP_ITR_SPACE;

			PyDict & dict = *((PyDict*)obj);
			fprintf(fp, "PyDict:%u\n", dict.size());
			PyDict::iterator itr = dict.begin();
			for (; itr != dict.end(); itr++)
			{
				PyDictEntry * entry = itr->second;
				WriteSpacer(fp, deep+DUMP_ITR_SPACE);

				/* write "dict[n]=" */
				fputs("dict[",fp);
					Dump(fp, entry->key, size_t(-1), true);
				fputs("]=",fp);
					Dump(fp, entry->obj, deep, true);
			}
		}
		break;

	case PyTypeTuple:
		{
			PyTuple & tuple = *((PyTuple*)obj);

			/* if our tuple is a itr, we need to read it as a part of a tuple/string/dict. So we need to add 2 spaces extra */
			if (isItr == true)
				deep+=DUMP_ITR_SPACE;

			fprintf(fp, "PyTuple:%u\n", tuple.size());

			for (uint32 i = 0; i < tuple.size(); i++)
			{
				WriteSpacer(fp, deep+DUMP_ITR_SPACE);
				fprintf(fp,"itr[%d]:", i);
				PyObject* item = tuple[i].getPyObject();
				
				/* this check only exists because of not complete DBRow unmarshal algorithm */
				if (item == NULL)
				{
					fputc('\n', fp);
					continue;
				}
				/* commented because: see above comments */
				//assert(item);
				Dump(fp, item, deep, true);
			}
		}
		break;

	case PyTypeList:
		{
			PyList & list = *((PyList*)obj);

			/* if our tuple is a itr, we need to read it as a part of a tuple/string/dict. So we need to add 2 spaces extra */
			if (isItr == true)
				deep+=DUMP_ITR_SPACE;

			fprintf(fp, "PyList:%u\n", list.size());
            for (PyList::iterator itr = list.begin(); itr != list.end(); itr++)
			{
				WriteSpacer(fp, deep+DUMP_ITR_SPACE);
				fprintf(fp,"itr:");
				Dump(fp, *itr, deep, true);
			}
		}
		break;

	case PyTypeSubStream:
		{
			PySubStream * stream = (PySubStream*) obj;
			fputs("PySubStream\n", fp);
			// make this optional, also make this so that you can auto decode this...

#if 1
			HexAsciiModule::print_hexview(fp, (const char*)stream->content(), stream->size());
#else

			ReadStream *rawsubstream = new ReadStream((const char*)stream->content(), stream->size());

			MarshalStream *substream = new MarshalStream();
			PyObject* obj1 = substream->load(*rawsubstream);
			if (obj1 != NULL)
			{
#ifdef MARSHAL_REMARSHAL
				WriteStream writeStream(rawsubstream->size());
				bool saveret = substream->save(obj1, writeStream);

				if (saveret == false)
				{
					DumpObject(stdout, obj1);
					HexAsciiModule::print_hexview(stdout, (char*)rawsubstream->content(), rawsubstream->size());
					HexAsciiModule::print_hexview(stdout, (char*)writeStream.content(), writeStream.size());
					ASCENT_HARDWARE_BREAKPOINT;
				}

				if(memcmp(writeStream.content(), rawsubstream->content(), rawsubstream->size()))
				{
					DumpObject(stdout, obj1);
					HexAsciiModule::print_hexview(stdout, (char*)rawsubstream->content(), rawsubstream->size());
					HexAsciiModule::print_hexview(stdout, (char*)writeStream.content(), writeStream.size());
					//ASCENT_HARDWARE_BREAKPOINT;
				}
#endif//MARSHAL_REMARSHAL
				Dump(fp, obj1, deep + 4);
				obj1->DecRef();
			}
			else
				ASCENT_HARDWARE_BREAKPOINT;

			if (rawsubstream->tell()!= rawsubstream->size())
				Log.Warning("PyDumper","sub stream isn't parsed completely: %u - %u", rawsubstream->tell(), rawsubstream->size());

			SafeDelete(substream);
			SafeDelete(rawsubstream);
#endif
		}
		break;

	case PyTypeClass:
		{
			PyClass * klass = (PyClass*) obj;

			fputs("PyClass\n", fp);

			/* handle special cases we need a different format dump */
			if (DBDumpModule::HandleSpecialDump(fp, deep, klass) == true)
				break;

			/* if our class is a itr, we need to read it as a part of a tuple/string/dict. So we need to add 2 spaces extra */
			if (isItr == true)
				deep+=DUMP_ITR_SPACE;

			if (klass->getname())
				Dump(fp, (PyObject*)klass->getname(), deep+DUMP_ITR_SPACE);

			if (klass->getbases())
				Dump(fp, (PyObject*)klass->getbases(), deep+DUMP_ITR_SPACE);

			if (klass->getdict())
				Dump(fp, (PyObject*)klass->getdict(), deep+DUMP_ITR_SPACE);

			if (klass->getDirDict())
				Dump(fp, (PyObject*)klass->getDirDict(), deep+DUMP_ITR_SPACE);

			if(klass->getDirList())
				Dump(fp, (PyObject*)klass->getDirList(), deep+DUMP_ITR_SPACE);
		}
		break;

	case PyTypeDeleted:
		assert(false); // this should never happen... fact.....
		break;

	case PyTypePackedRow:
		{
			PyPackedRow * row = (PyPackedRow*) obj;
	
			fputs("PyPackedRow\n", fp);
			/* if our class is part of a itr dump make sure we format it */
			if (isItr == true)
				deep+=DUMP_ITR_SPACE;

			fprintf(fp, "rawFieldData: size:%u\n", row->mRawFieldDataLen);
			HexAsciiModule::print_hexview(fp, (char*)row->mRawFieldData, row->mRawFieldDataLen);
			fprintf(fp, "\n");

			assert(row->getheader());
			Dump(fp, (PyObject*)row->getheader(), deep+DUMP_ITR_SPACE);

			if(isItr == false)
				WriteSpacer(fp, deep);

			PyObject * rawTuple = row->getRawPayLoad();
			if (rawTuple != NULL)
			{
				fputs("Raw fields:\n", fp);
				Dump(fp, rawTuple, deep, true);

				if(isItr == false)
					WriteSpacer(fp, deep);
			}

			if (row->size() != 0)
				fputs("Virtual fields:\n", fp);

			deep+=DUMP_ITR_SPACE;

			PyPackedRow::iterator Itr = row->begin();
			for (; Itr != row->end(); Itr++)
			{
				Dump(fp, (PyObject*)(*Itr)->getPyObject(), deep);
			}
			fputc('\n', fp);
		}
		break;

	case PyTypeSubStruct:
		{
			PySubStruct * substruct = (PySubStruct*) obj;

			fputs("PySubStruct\n", fp);

			/* if our class is part of a itr dump make sure we format it */
			if (isItr == true)
				deep+=DUMP_ITR_SPACE;

			assert(substruct->getPyObject());
			if (substruct->getPyObject())
				Dump(fp, (PyObject*)substruct->getPyObject(), deep+DUMP_ITR_SPACE);
		}
		break;

	default:
		printf("PyDump whelp: unhandled opcode: %u from object: 0x%p", obj->gettype(), obj);
		assert(false);
		break;
	}

	deep++;
}

/* small utility function */
inline int canprint(unsigned char character)
{
	return ((character > 31) && (character < 127));
}

bool IsPrintString(const char* str, const size_t len)
{
	if (len == 0 || str == NULL)
		return false;

	unsigned char * src = (unsigned char *)str;
	size_t isReadableCount = 0;

	size_t doubleLoop = len / 2;
	uint8 rest = len % 2;

	while(doubleLoop--)
	{
		isReadableCount+=canprint(*src);
		src+=2;
	}

	if (rest)
	{
		src++;
		isReadableCount+=canprint(*src);
	}

	if (float(isReadableCount) / float(len/2) < 0.80)
		return false;
	
	return true;
}

void WriteSpacer(FILE * fp, size_t count)
{
	if (count == 0)
		return;

	size_t doubleLoop = count / 2;
	uint8 rest = count % 2;

	/* performance wise write 2 characters at the time */
	while(doubleLoop--)
		fputs("  ", fp);

	if(rest)
		fputc(' ', fp);
}

bool IsPrintString(PyString * obj)
{
	return IsPrintString(obj->content(), obj->length());
}
