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
#include "DBRowModule.h"
#include "PyObjectDumper.h"

class sortInfo
{
public:
    sortInfo() : typeSize(0), index(0) {}
	sortInfo(uint8 xtypeSize, uint32 xindex)
	{
		typeSize = xtypeSize;
		index = xindex;
	}

/* not protected for a reason */
	uint8 typeSize;
	uint32 index;
};

bool DBRowSortCallback (sortInfo &i,sortInfo &j)
{ 
	return (i.typeSize > j.typeSize);
}

/* this stuff should be handled by the dbrowdescriptor I think.... I have no clue... */
PyObject* DBRowModule::parseraw( MarshalStream& stream, PyObject* header, uint8 * data, size_t len, size_t & virtualFieldCount, size_t & fieldCount )
{
	mReadIndex = 0;
	mReadBufferSize = len;
	mReadBuffer = data;
	mVirtualFieldCount = 0;

	PyClass * bas = (PyClass*)header;
	PyTuple & classdata = *bas->getbases();

	if (bas->getbases() == NULL)
	{
		printf("hack stuff dbrowlist stuff \n");
		Dump(stdout, (PyObject*)bas, 0);
		return NULL;
	}

	assert(PyTuple_Check(&classdata));

	PyTuple& classdataterror = *classdata.get_item_tuple(1);
	PyTuple& tuple = *classdataterror.get_item_tuple(0);

	if (tuple.size() == 0)
		ASCENT_HARDWARE_BREAKPOINT;

	PyTuple & result_tuple = *PyTuple_New(tuple.size());

	std::vector<sortInfo> henk;
	size_t henks = tuple.size();
	henk.resize(henks);

	for (int i = 0; i < (int)tuple.size(); i++)
	{
		PyTuple& field = *tuple.get_item_tuple(i);
		uint32 type = field.get_item_int(1);

		henk[i].index = i;
		henk[i].typeSize = (uint8)GetFieldSizeFromType(type);
	}	

	std::sort(henk.begin(), henk.end(), DBRowSortCallback);

	for (int i = 0; i < (int)tuple.size(); i++)
	{
		PyTuple& field = (PyTuple&)*tuple.get_item(henk[i].index);

		/* this tuple is required to have 2 leaves because it has a field name and a field type */
		assert(field.size() == 2); // needs to be 2

		PyInt& fieldType = (PyInt&)*field.get_item(1);
		PyObject* resObj = ReadRawDbField(stream, fieldType, mVirtualFieldCount);

		if (resObj == NULL)
			continue;

		result_tuple.set_item(henk[i].index, resObj);
	}

	virtualFieldCount = mVirtualFieldCount;
	fieldCount = tuple.size();
	return (PyObject*)&result_tuple;
}

PyObject* DBRowModule::ReadRawDbField( MarshalStream& stream, PyInt &type, size_t &virtualFieldCount )
{
#define CHECK_SIZE(size) if(mReadIndex + size > mReadBufferSize) {\
	/*HexAsciiModule::print_hexview(stdout, (char*)mReadBuffer, mReadBufferSize);*/\
	assert(mReadIndex + size <= mReadBufferSize); }
#define MARSHALSTREAM_RETURN(p) {PyObject * x = ((PyObject*)p); assert(x != NULL); return x;}
//#define DBROW_DEBUG_FIELD_LOG(x) printf("%s, index: 0x%X\n", #x, mReadIndex)
#define DBROW_DEBUG_FIELD_LOG(x) /*x*/

	if (mReadIndex > mReadBufferSize)
	{
		ASCENT_HARDWARE_BREAKPOINT;
		PyIncRef(&stream.PyNone);
		return (PyObject*)&stream.PyNone;
	}

	switch(type.get_value())
	{
	case DBTYPE_I1:
		{
			DBROW_DEBUG_FIELD_LOG(DBTYPE_I1);
			CHECK_SIZE(1);
			int8 value = 0;
			value = *(int8*)&mReadBuffer[mReadIndex];
			mReadIndex++;
			MARSHALSTREAM_RETURN(new PyInt(value));
		} break;
	case DBTYPE_UI1:
		{
			DBROW_DEBUG_FIELD_LOG(DBTYPE_UI1);
			CHECK_SIZE(1);
			uint8 value = 0;
			value = mReadBuffer[mReadIndex];
			mReadIndex++;
			MARSHALSTREAM_RETURN(new PyInt(value));
		} break;
	case DBTYPE_BOOL: 
		{
			DBROW_DEBUG_FIELD_LOG(DBTYPE_BOOL);
			CHECK_SIZE(1);
			uint8 value = 0;
			value = mReadBuffer[mReadIndex];
			mReadIndex++;
			MARSHALSTREAM_RETURN(new PyBool(value != 0));
		} break;
	case DBTYPE_I2: 
		{
			DBROW_DEBUG_FIELD_LOG(DBTYPE_I2);
			CHECK_SIZE(2);
			int16 value = 0;
			value = *((int16*)&mReadBuffer[mReadIndex]);
			mReadIndex+=2;
			MARSHALSTREAM_RETURN(new PyInt(value));
		} break;
	case DBTYPE_UI2: 
		{
			DBROW_DEBUG_FIELD_LOG(DBTYPE_UI2);
			CHECK_SIZE(2);
			uint16 value = 0;
			value = *((uint16*)&mReadBuffer[mReadIndex]);
			mReadIndex+=2;
			MARSHALSTREAM_RETURN(new PyInt(value));
		} break;
	case DBTYPE_I4: 
		{
			DBROW_DEBUG_FIELD_LOG(DBTYPE_I4);
			CHECK_SIZE(4);
			int32 value = 0;
			value = *((int32*)&mReadBuffer[mReadIndex]);
			mReadIndex+=4;
			MARSHALSTREAM_RETURN(new PyInt(value));
		} break;
	case DBTYPE_UI4:
		{
			DBROW_DEBUG_FIELD_LOG(DBTYPE_UI4);
			CHECK_SIZE(4);
			uint32 value = 0;
			value = *((uint32*)&mReadBuffer[mReadIndex]);
			mReadIndex+=4;
			MARSHALSTREAM_RETURN(new PyInt(value));
		} break;
	case DBTYPE_R4: 
		{
			DBROW_DEBUG_FIELD_LOG(DBTYPE_R4);
			CHECK_SIZE(4);
			ASCENT_HARDWARE_BREAKPOINT;
		} break;
	case DBTYPE_I8:
		{
			DBROW_DEBUG_FIELD_LOG(DBTYPE_I8);
			CHECK_SIZE(8);
			int64 value = 0;
			value = *((int64*)&mReadBuffer[mReadIndex]);
			mReadIndex+=8;
			MARSHALSTREAM_RETURN(new PyLong(value));
		} break;
	case DBTYPE_R8:
		{
			DBROW_DEBUG_FIELD_LOG(DBTYPE_R8);
			CHECK_SIZE(8);
			double * value = (double*)&mReadBuffer[mReadIndex];
			mReadIndex+=8;
			MARSHALSTREAM_RETURN(new PyFloat(*value));
		} break;

	case DBTYPE_FILETIME:
	case DBTYPE_UI8:
		{
			DBROW_DEBUG_FIELD_LOG(DBTYPE_UI8);
			CHECK_SIZE(8);
			uint64 value = 0;
			value = *((uint64*)&mReadBuffer[mReadIndex]);
			mReadIndex+=8;
			MARSHALSTREAM_RETURN(new PyLong(value));
		} break;

	case DBTYPE_CY: 
		{
			DBROW_DEBUG_FIELD_LOG(DBTYPE_CY);
			CHECK_SIZE(8);

			uint64 henk = *((uint64*)&mReadBuffer[mReadIndex]);
			double sjaak = (double)henk;
			sjaak /= 10000.0;
			
			mReadIndex+=8;
			MARSHALSTREAM_RETURN(new PyFloat(sjaak));
		} break;

	case DBTYPE_BYTES:
	case DBTYPE_STR:
	case DBTYPE_WSTR:
		{
			DBROW_DEBUG_FIELD_LOG(DBTYPE_BYTES);
			virtualFieldCount++;
			return NULL;
		} break;

	default:
		//assert(false);
		ASCENT_HARDWARE_BREAKPOINT;
		break;
	}
	ASCENT_HARDWARE_BREAKPOINT;
	return NULL;
}

size_t DBRowModule::GetRawFieldSizeFromHeader( PyObject* object )
{
    PyTuple * pTuple = (PyTuple*)object;

	if (!PyTuple_Check(pTuple))
		return size_t(-1);
	
	if (pTuple->size() == 0)
		return size_t(-1);

	/* check if we need to get the sub object */
	if (pTuple->size() == 1)
	{
		PyObject* subObject = pTuple->get_item(0);
		if (!PyTuple_Check(subObject))
			return size_t(-1);

		pTuple = (PyTuple*)subObject;
	}

	size_t chunkSize = 0;
	for (int i = 0; i < (int)pTuple->size(); i++)
	{
		PyTuple * leaf = pTuple->get_item_tuple(i);
		if (leaf->size() != 2)
			ASCENT_HARDWARE_BREAKPOINT;

		uint32 fieldtype = leaf->get_item_int(1);
		uint8 fieldSize = (uint8)GetFieldSizeFromType(fieldtype);
		chunkSize+=fieldSize;
		//printf("db row field type = %u, size = %u\n", fieldtype, fieldSize);
	}
	//printf("db row count end\n");
	return chunkSize;
}

size_t DBRowModule::GetFieldSizeFromType( uint8 type )
{
	switch(type) {
		case DBTYPE_I8:
		case DBTYPE_R8:
		case DBTYPE_UI8:
		case DBTYPE_CY:
		case DBTYPE_FILETIME:
			return 8;
		case DBTYPE_I4:
		case DBTYPE_UI4:
		case DBTYPE_R4:
			return 4;
		case DBTYPE_I2:
		case DBTYPE_UI2:
			return 2;
		case DBTYPE_I1:
		case DBTYPE_UI1:
		case DBTYPE_BOOL:
			return 1;

			/* these are added as PySequence iterators behind the raw data field */
		case DBTYPE_BYTES:
		case DBTYPE_STR:
		case DBTYPE_WSTR:
			return 0;
	}
	ASCENT_HARDWARE_BREAKPOINT;
	return 0;
}
