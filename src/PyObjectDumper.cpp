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

void DumpObject(FILE * fd, PyObject* obj)
{
	Dump(fd, obj, 0);
	fflush(fd);
}

#define DUMP_ITR_SPACE 2

void Dump( FILE * fp, PyObject * obj, int deep, bool isItr /*= false*/, bool dict_trick /*= false*/ )
{
	if (obj == NULL)
	{
		printf("Can not dump NULL object\n");
		return;
	}
	//printf("Deep: %d, PyObject type: 0x%X, filtered: 0x%X\n", deep, obj->get_type(), obj->get_type());

	/* this one makes sure that we have slightly formatted output (important for readability) */
	if(isItr == false && deep != -1)
		WriteSpacer(fp, deep);

	switch(obj->get_type())
	{
	case PyTypeNone:
		//if (dict_trick == false)
			fputs("PyNone", fp);
		//else
		//	fputs("NULL", fp);

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

			int32 val = num.get_value();
			//if ( val < 10 && val > -10 || deep == -1)
				fprintf(fp, "%d", val);
			//else
			//	fprintf(fp, "%d    <0x%X>", num.get_value(), num.get_value());

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

			int64 val = num.get_value();
			//if ( val < 10 && val > -10 || deep == -1)
				fprintf(fp, I64FMTD, val);
			//else
			//	fprintf(fp, I64FMTD"    <0x%I64X>", num.get_value(), num.get_value());

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

			fprintf(fp, "%f", num->get_value());

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
						Dump(fp, obj1, deep+4);
						PyDecRef(obj1);
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

			if (str->size() == 0 || str->content() == NULL)
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

            if (dict_trick == false)
            {
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
            else
            {
                //dict({'one': 1, 'two': 2})
                PyDict & dict = *((PyDict*)obj);
                fprintf(fp, "dict(");

                PyDict::iterator itr = dict.begin();
                for (; itr != dict.end(); itr++)
                {
                    PyDictEntry * entry = itr->second;
                    //WriteSpacer(fp, deep+DUMP_ITR_SPACE);

                    /* write "dict[n]=" */
                    Dump(fp, entry->key, size_t(-1), true);
                    fputs(": ",fp);
                    Dump(fp, entry->obj, size_t(-1), true, true);

                    /* formatting hacks */
                    PyDict::iterator itr2 = itr;
                    itr2++;

                    if(itr2 != dict.end())
                        fputs(", ",fp);
                }

                fprintf(fp, ")");

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
				PyObject* item = tuple.get_item(i);
				
				/* this check only exists because of not complete DBRow unmarshal algorithm */
				if (item == NULL) {
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
            fprintf(fp, "PySubStream size:%u <hidden from dump>\n", stream->size());
			// make this optional, also make this so that you can auto decode this...

#if 1
			//HexAsciiModule::print_hexview(fp, (const char*)stream->content(), stream->size());
#else

			ReadStream *rawsubstream = new ReadStream((const char*)stream->content(), stream->size());

			MarshalStream *substream = new MarshalStream();
			PyObject* obj1 = substream->load(*rawsubstream);
			if (obj1 != NULL)
			{
				Dump(fp, obj1, deep + 4);
				PyDecRef(obj1);
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
			PyClass * pyclass_obj = (PyClass*) obj;

            if (!pyclass_obj->repr(fp))
                printf("repr of the object has failed\n");
		}
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
				Dump(fp, (*Itr), deep);

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
		printf("PyDump whelp: unhandled opcode: %u from object: 0x%p", obj->get_type(), obj);
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
