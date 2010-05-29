#include "EvemuPCH.h"
#include "DBRowModule.h"
#include "PyObjectDumper.h"

struct sortInfo
{
	sortInfo(){}
	sortInfo(uint8 xtypeSize, uint32 xindex)
	{
		typeSize = xtypeSize;
		index = xindex;
	}

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

	assert(classdata.gettype() == PyTypeTuple);

	PyTuple& classdataterror = *classdata.GetItem_asPyTuple(1);
	PyTuple& tuple = *classdataterror.GetItem_asPyTuple(0);

	if (tuple.size() == 0)
		ASCENT_HARDWARE_BREAKPOINT;

	PyTuple & result_tuple = *PyTuple_New(tuple.size());

	std::vector<sortInfo> henk;
	size_t henks = tuple.size();
	henk.resize(henks);

	for (int i = 0; i < (int)tuple.size(); i++)
	{
		PyTuple& field = *tuple.GetItem_asPyTuple(i);
		uint32 type = field.GetItem_asInt(1);
		uint32 index = i;

		henk[i].index = i;
		henk[i].typeSize = GetFieldSizeFromType(type);
	}	

	std::sort(henk.begin(), henk.end(), DBRowSortCallback);

	for (int i = 0; i < (int)tuple.size(); i++)
	{
		PyTuple& field = (PyTuple&)tuple[henk[i].index].getPyObjRef();

		/* this tuple is required to have 2 leaves because it has a field name and a field type */
		assert(field.size() == 2); // needs to be 2

		PyInt& fieldType = (PyInt&)field[1].getPyObjRef();
		PyObject* resObj = ReadRawDbField(stream, fieldType, mVirtualFieldCount);

		if (resObj == NULL)
			continue;

		result_tuple[henk[i].index] = resObj;
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
		stream.PyNone.IncRef();
		return (PyObject*)&stream.PyNone;
	}

	switch(type.GetValue())
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
	if (!PY_TYPE_CHECK(object, PyTypeTuple))
		return size_t(-1);

	PyTuple * pTuple = (PyTuple*)object;
	if (pTuple->size() == 0)
		return size_t(-1);

	/* check if we need to get the sub object */
	if (pTuple->size() == 1)
	{
		PyObject* subObject = pTuple->GetItem(0);
		if (!PY_TYPE_CHECK(subObject, PyTypeTuple))
			return size_t(-1);

		pTuple = (PyTuple*)subObject;
	}

	size_t chunkSize = 0;
	for (int i = 0; i < (int)pTuple->size(); i++)
	{
		PyTuple * leaf = pTuple->GetItem_asPyTuple(i);
		if (leaf->size() != 2)
			ASCENT_HARDWARE_BREAKPOINT;

		uint32 fieldtype = leaf->GetItem_asInt(1);
		uint8 fieldSize = GetFieldSizeFromType(fieldtype);
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
