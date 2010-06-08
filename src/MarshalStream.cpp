#include "Common.h"
#include "ascent.h"

#include "EvemuPCH.h"

#include "EveMarshalOpcodes.h"
#include "PyStringTable.h"
#include "PyCallObject.h"

#include <zlib.h>

#include "DBRowModule.h"
#include "PyObjectDumper.h"

/* macro's that help debugging exceptions */
#define EVEMU_EXTRA_DEBUG
#ifdef _DEBUG
#  ifdef EVEMU_EXTRA_DEBUG
#    define MARSHALSTREAM_RETURN_NULL {ASCENT_HARDWARE_BREAKPOINT; return NULL;}
#    define MARSHALSTREAM_RETURN(p) {PyObject * x = ((PyObject*)p); assert(x != NULL); return x;}
//#  define MARSHALSTREAM_RETURN(p) {PyObject * x = ((PyObject*)p); assert(x != NULL); Dump(stdout, (PyObject*)x, 0); return x;}
//#  define MARSHALSTREAM_RETURN(p) {PyObject * x = ((PyObject*)p); assert(x != NULL); Dump(fp_debug, (PyObject*)x, 0); return x;}
#  else
#    define MARSHALSTREAM_RETURN_NULL return NULL;
#    define MARSHALSTREAM_RETURN(p) return (PyObject*)p;
# endif//EVEMU_EXTRA_DEBUG
#else
#  ifdef EVEMU_EXTRA_DEBUG
#    define MARSHALSTREAM_RETURN_NULL {sLog.String(__FUNCTION__" :returning NULL"); return NULL;}
#    define MARSHALSTREAM_RETURN(p) { PyObject * x = ((PyObject*)p); if (x == NULL) {sLog.Error(__FUNCTION__" :returning NULL");} return x; }
#  else
#    define MARSHALSTREAM_RETURN_NULL return NULL;
#    define MARSHALSTREAM_RETURN(p) return (PyObject*)p;
#  endif//EVEMU_EXTRA_DEBUG
#endif//_DEBUG

/* macro's that help debugging exceptions */
#ifdef _DEBUG
#  define unmarshalState(x, y) {sLog.String("State:"#x"\toffset:0x%X", y.tell());}
//#  define unmarshalState(x, y) /*{x, y}*/
#else
#  define unmarshalState(x, y) /*{x, y}*/
#endif//_DEBUG

MarshalStream::MarshalStream() : PyIntZero(0), PyIntOne(1), PyIntMinOne(-1), PyFloatZero(0.0),PyIntMinusOne(-1),
	Py_TrueStruct(true), Py_ZeroStruct(false) {}

MarshalStream::~MarshalStream() {}

PyObject* MarshalStream::load(ReadStream & stream)
{
	/* first time check for byte code because I don't want to contaminate checkAndInflate */
	if (checkForBytecode(stream) == true)
		return NULL;

	if (checkAndInflate(stream) == false)
		MARSHALSTREAM_RETURN_NULL;

	/* second time check for byte code because its possible it was zipped */
	if (checkForBytecode(stream) == true)
		return NULL;

	if (ReadMarshalHeader(stream) == false)
	{
		Log.Error("MarshalStream", "[load] Unable to read and initialize the packet header");
		return NULL;
	}

	PyObject* unmarshaledData = unmarshal(stream);
	MARSHALSTREAM_RETURN(unmarshaledData);
}

bool MarshalStream::ReadMarshalHeader( ReadStream & stream )
{
	char marshalTilde;
	if (!stream.read1(marshalTilde))
	{
		Log.Error("MarshalStream", "[ReadMarshalHeader]Can't read %d elements of %d bytes, only have %d bytes left", 1, 1, stream.size() - stream.tell());
		return false;
	}

	if (marshalTilde != '~')
	{
		//Log.Error("MarshalStream", "[ReadMarshalHeader]invalid marshal header, missing tilde");
		Log.Error("MarshalStream", "[ReadMarshalHeader]invalid marshal header, missing tilde: 0x%X", marshalTilde);
		return false;
	}

	int32 sharedObjectCount;
	if (!stream.read4(sharedObjectCount))
	{
		Log.Error("MarshalStream", "[ReadMarshalHeader]Can't read %d elements of %d bytes, only have %d bytes left", 1, 4, stream.size() - stream.tell());
		return false;
	}

	/* check if this packet contains referenced objects */
	if (sharedObjectCount <= 0)
		return true;

	mReferencedObjectsMap.SetOrderMapSize(sharedObjectCount);
	mReferencedObjectsMap.SetSharedObjectCount(sharedObjectCount);
	
	size_t tReadIndex2 = stream.tell();
	if ( (signed int)(stream.size() - tReadIndex2) / 4 < sharedObjectCount )
	{
		Log.Error("MarshalStream", "[ReadMarshalHeader]Too little data in marshal stream, %u bytes. I really wanted at least %u bytes total, mapcount in header is %d", stream.size(), tReadIndex2 + 4 * sharedObjectCount, sharedObjectCount);
		return false;
	}

	stream.setpayloadsize(stream.size() - (4 * sharedObjectCount));

	size_t sharedObjectIndex = stream.buffersize() - 4 * sharedObjectCount;

	int32* sharedObjectBegin = (int32*)&stream.content()[sharedObjectIndex];
	int32 loopcounter = 0;
	while ( *sharedObjectBegin >= 1 && *sharedObjectBegin <= sharedObjectCount )
	{
		mReferencedObjectsMap.SetObjectOrder(loopcounter, *sharedObjectBegin);
		++loopcounter;
		++sharedObjectBegin;
		
		if ( loopcounter >= sharedObjectCount )
			return true;
	}

	Log.Error("MarshalStream", "Bogus map data in marshal stream.");

	return false;
}

PyObject* MarshalStream::unmarshal( ReadStream & stream )
{
	if (stream.size() - stream.tell() >= 1)
	{
		uint8 opcode;
		stream.read1(opcode);
		if (((opcode >> 6) & 0xFFFFFF01) != 0)
			printf("ref obj: [0X%x]\n", opcode & 0x3F);

		switch ( opcode & 0x3F )
		{
			case Op_PyNone:
			{
				unmarshalState(Op_PyNone, stream);
				PyNone.IncRef();
				MARSHALSTREAM_RETURN(&PyNone);
			}

            case Op_PyModule:
			{
				unmarshalState(Op_PyModule, stream);
				MARSHALSTREAM_RETURN(ReadClassString(stream, (opcode & 0x40) != 0));
			}

			case Op_PyLongLong:
			{
				unmarshalState(Op_PyLongLong, stream);
				int64 number;
				if (!stream.read8(number))
					MARSHALSTREAM_RETURN_NULL;
				MARSHALSTREAM_RETURN(PyLong_FromLongLong(number));
			}

			case Op_PyLong:
			{
				unmarshalState(Op_PyLong, stream);
				int32 number;
				if (!stream.read4(number))
					MARSHALSTREAM_RETURN_NULL;
				MARSHALSTREAM_RETURN(PyInt_FromLong(number));
			}

			case Op_PySignedShort:
			{
				unmarshalState(Op_PySignedShort, stream);
				int16 number;
				if (!stream.read2(number))
					MARSHALSTREAM_RETURN_NULL;
				MARSHALSTREAM_RETURN(PyInt_FromLong(number));
			}

			case Op_PyByte:
			{
				unmarshalState(Op_PyByte, stream);
				int8 number;
				if (!stream.read1(number))
					MARSHALSTREAM_RETURN_NULL;
				MARSHALSTREAM_RETURN(PyInt_FromLong(number));
			}

			case Op_PyMinusOne:
			{
				unmarshalState(Op_PyMinusOne, stream);
				PyIntMinusOne.IncRef();
				MARSHALSTREAM_RETURN(&PyIntMinusOne);
			}
			
			case Op_PyZeroInteger:
			{
				unmarshalState(Op_PyZeroInteger, stream);
				PyIntZero.IncRef();
				MARSHALSTREAM_RETURN(&PyIntZero);
			}
			
			case Op_PyOneInteger:
			{
				unmarshalState(Op_PyOneInteger, stream);
				PyIntOne.IncRef();
				MARSHALSTREAM_RETURN(&PyIntOne);
			}
			
			case Op_PyFloat:
			{
				unmarshalState(Op_PyFloat, stream);
				double number;
				if (!stream.read8(number))
					MARSHALSTREAM_RETURN_NULL;
				MARSHALSTREAM_RETURN(PyFloat_FromDouble(number));
			}

			case Op_PyZeroFloat:
			{
				unmarshalState(Op_PyZeroFloat, stream);
				PyFloatZero.IncRef();
				MARSHALSTREAM_RETURN(&PyFloatZero);
			}
			
			case Op_PyTrue:
			{
				unmarshalState(Op_PyTrue, stream);
				Py_TrueStruct.IncRef();
				MARSHALSTREAM_RETURN(&Py_TrueStruct);
			}
			
			case Op_PyFalse:
			{
				unmarshalState(Op_PyFalse, stream);
				Py_ZeroStruct.IncRef();
				MARSHALSTREAM_RETURN(&Py_ZeroStruct);
			}
			
			/* seems to be uncorrect */
			case Op_PyLongString:
			{
				ASCENT_HARDWARE_BREAKPOINT;
				unmarshalState(Op_PyLongString, stream);
				MARSHALSTREAM_RETURN(ReadBuffer(stream));
			}
			
			case Op_PyEmptyString:
			{
				unmarshalState(Op_PyEmptyString, stream);
				PyStringEmpty.IncRef();
				MARSHALSTREAM_RETURN(&PyStringEmpty);
			}
			
			case Op_PyCharString:
			{
				unmarshalState(Op_PyCharString, stream);
				char CharString;
				if (!stream.read1(CharString))
					MARSHALSTREAM_RETURN_NULL;
				MARSHALSTREAM_RETURN(PyString_FromStringAndSize(&CharString, 1));
			}

			case Op_PyShortString:
			{
				unmarshalState(Op_PyShortString, stream);
				uint8 strlen;
				char* strptr;
				if (!stream.read1(strlen))
					MARSHALSTREAM_RETURN_NULL;
				if (!stream.readString(&strptr, strlen))
					MARSHALSTREAM_RETURN_NULL;
				MARSHALSTREAM_RETURN(PyString_FromStringAndSize(strptr, strlen));
			}

			case Op_PyStringTableItem:
			{
				unmarshalState(Op_PyStringTableItem, stream);
				uint8 index;
				if (!stream.read1(index))
					MARSHALSTREAM_RETURN_NULL;
	
				PyString * ret = NULL;
				if (!sPyStringTable.LookupPyString(index, ret) && ret == NULL)
				{
					Log.Error("MarshalStream", "Invalid string table index %d", index);
					MARSHALSTREAM_RETURN_NULL;
				}
                Log.Warning("MarshalDebug", "string: %s", ret->content());
				MARSHALSTREAM_RETURN(ret);
			}

			case Op_PyUnicodeString:
			{
				unmarshalState(Op_PyUnicodeString, stream);
				uint32 strlen;
				wchar_t* strptr;
				if (!stream.readSizeEx(strlen))
					MARSHALSTREAM_RETURN_NULL;
	
				if (!stream.readWstring(&strptr, strlen))
					MARSHALSTREAM_RETURN_NULL;
	
                Log.Warning("MarshalDebug", "unicode string: %S", strptr);
				MARSHALSTREAM_RETURN(PyUnicodeUCS2_FromWideChar(strptr, strlen));
			}

			case Op_PyUnicodeEmptyString:
			{
				unmarshalState(Op_PyUnicodeEmptyString, stream);
				MARSHALSTREAM_RETURN(PyUnicodeUCS2_FromWideChar(0, 0));
			}

			case Op_PyUnicodeCharString:
			{
				unmarshalState(Op_PyUnicodeCharString, stream);
				wchar_t singleWChar;
				if (!stream.read2(singleWChar))
					MARSHALSTREAM_RETURN_NULL;
				MARSHALSTREAM_RETURN(PyUnicodeUCS2_FromWideChar(&singleWChar, 1));
			}

			case Op_PyUnicodeUTF8String:
			{
				unmarshalState(Op_PyUnicodeUTF8String, stream);
				uint32 strlen;
				char* strptr = NULL;
				if (!stream.readSizeEx(strlen))
					MARSHALSTREAM_RETURN_NULL;
	
				if (!stream.readString(&strptr, strlen))
					MARSHALSTREAM_RETURN_NULL;
	
				MARSHALSTREAM_RETURN(PyUnicodeUCS2_DecodeUTF8(strptr, strlen));
			}

			case Op_PyLongString_Shared:
			{
				unmarshalState(Op_PyLongString_Shared, stream);
				PyObject * str_obj = ReadBuffer(stream);
				if ((opcode & 0x40) != 0)
				{
					if(mReferencedObjectsMap.StoreReferencedObject(str_obj) == -1)
						MARSHALSTREAM_RETURN_NULL;
				}
				MARSHALSTREAM_RETURN(str_obj);
			}

			case Op_PyEmptyTuple:
			{
				unmarshalState(Op_PyEmptyTuple, stream);
				MARSHALSTREAM_RETURN(PyTuple_New(0));
			}
			
			case Op_PyOneTuple:
			{
				unmarshalState(Op_PyOneTuple, stream);
				PyTuple& tuple = *PyTuple_New(1);
				if ((opcode & 0x40) != 0)
				{
					if(mReferencedObjectsMap.StoreReferencedObject(&tuple) == -1)
						MARSHALSTREAM_RETURN_NULL;
				}
	
				// warning recursive function...
				PyObject *object = unmarshal(stream);
				assert(object != (PyObject*)&tuple);
				tuple.set_item(0, object);
				MARSHALSTREAM_RETURN(&tuple);
			}

			case Op_PyTwoTuple:
			{
				unmarshalState(Op_PyTwoTuple, stream);
				PyTuple& tuple = *PyTuple_New(2);
				if ((opcode & 0x40) != 0)
				{
					if(mReferencedObjectsMap.StoreReferencedObject(&tuple) == -1)
						MARSHALSTREAM_RETURN_NULL;
				}
	
				// warning double recursive function...
				for (int i = 0; i < 2; i++)
				{
					PyObject* Itr = unmarshal(stream);
					assert(Itr != (PyObject*)&tuple);
					tuple.set_item(i, Itr);
				}

				MARSHALSTREAM_RETURN(&tuple);
			}
			
			case Op_PyTuple:
			{
				unmarshalState(Op_PyTuple, stream);
				uint32 elementCount;
				if (!stream.readSizeEx(elementCount))
					MARSHALSTREAM_RETURN_NULL;
	
				PyTuple& tuple = *PyTuple_New(elementCount);
				if ((opcode & 0x40) != 0)
				{
					if(mReferencedObjectsMap.StoreReferencedObject(&tuple) == -1)
						MARSHALSTREAM_RETURN_NULL;
				}
	
				// warning double recursive function...
				for (uint32 i = 0; i < elementCount; i++)
				{
					PyObject* Itr = unmarshal(stream);
					assert(Itr != (PyObject*)&tuple);
					tuple.set_item(i, Itr);
				}

				MARSHALSTREAM_RETURN(&tuple);
			}

			case Op_PyEmptyList:
			{
				unmarshalState(Op_PyEmptyList, stream);
				MARSHALSTREAM_RETURN(PyList_New(0));
			}

			case Op_PyOneList:
			{
				unmarshalState(Op_PyOneList, stream);
				PyList &list = *PyList_New(1);
				if (&list == NULL)
					MARSHALSTREAM_RETURN_NULL;
				
				if ((opcode & 0x40) != 0)
				{
					if(mReferencedObjectsMap.StoreReferencedObject(&list) == -1)
						MARSHALSTREAM_RETURN_NULL;
				}
				// recursive function...
				list[0] = unmarshal(stream);
				assert(list[0].getPyObject() != (PyObject*)&list);

				MARSHALSTREAM_RETURN(&list);
			}

			case Op_PyList:
			{
				unmarshalState(Op_PyList, stream);
				uint32 elementCount;
				if (!stream.readSizeEx(elementCount))
					MARSHALSTREAM_RETURN_NULL;
	
				PyList &list = *PyList_New(elementCount);
				if (&list == NULL)
					MARSHALSTREAM_RETURN_NULL;
	
				if ((opcode & 0x40) != 0)
				{
					if(mReferencedObjectsMap.StoreReferencedObject(&list) == -1)
						MARSHALSTREAM_RETURN_NULL;
				}
	
				// n recursive function call
				for (uint32 i = 0; i < elementCount; i++)
				{
					PyObject* itr = unmarshal(stream);
					assert(itr != (PyObject*)&list);
					if (itr == NULL)
						MARSHALSTREAM_RETURN_NULL;

					list[i] = itr;
				}
				MARSHALSTREAM_RETURN(&list);
			}
			
			/* this handler is a lot more complicated than I have implemented here... will do for now... */
			case Op_PyDict:
			{
				unmarshalState(Op_PyDict, stream);
				uint32 elementCount;
				if (!stream.readSizeEx(elementCount))
					MARSHALSTREAM_RETURN_NULL;
	
				PyDict & dict = *PyDict_New();
				if ((opcode & 0x40) != 0)
				{
					if(mReferencedObjectsMap.StoreReferencedObject(&dict) == -1)
						MARSHALSTREAM_RETURN_NULL;
				}
	
				// whoo tricky stuff....
				for (uint32 i = 0; i < elementCount; i++)
				{
					PyObject* keyPayload = unmarshal(stream);			// Payload
					PyObject* keyName = unmarshal(stream);				// the keyname

					assert(keyName);

					dict.set_item(keyName, keyPayload);

					keyPayload->DecRef();
					keyName->DecRef();
				}

				MARSHALSTREAM_RETURN(&dict);
			}

			/* read a object, it also has a check for reference mapping */
			case Op_PyGlobalObject:
			{
                //ASCENT_HARDWARE_BREAKPOINT;
				unmarshalState(Op_PyGlobalObject, stream);
				MARSHALSTREAM_RETURN(ReadGlobalInstance(stream, (opcode >> 6) & 1));
			}

			/* unknown isn't handled but what we know of them is that its related to the cPickle system */
			case Op_PyBinairy:
			{
				unmarshalState(Op_PyBinairy, stream);
				MARSHALSTREAM_RETURN_NULL;
			}
			
			/* need to implement custom callbacks and reading stuff... but for the server this doesn't seem to usefull.. */
			case Op_PyClassInstance:
			{
				/* Unmarshal stream contains custom data but I have no callback method */
                ASCENT_HARDWARE_BREAKPOINT;

				unmarshalState(Op_PyClassInstance, stream);
				PySubStruct * obj = new PySubStruct();
				PyObject * tobj = unmarshal(stream);
				assert(obj->setPyObject(tobj));
				tobj->DecRef();
                // PyObject_CallFunctionObjArgs
				
				MARSHALSTREAM_RETURN(obj);
			}

			/**/
			case Op_PyLoadcPickledObject:
			{
                ASCENT_HARDWARE_BREAKPOINT;
				unmarshalState(Op_PyLoadcPickledObject, stream);
				/* |extended size|stream| */
				//MARSHALSTREAM_RETURN(PyObject_CallMethod(unpickledObject, "load", 0);
				MARSHALSTREAM_RETURN_NULL;
			}
		
			case Op_cPicked:
			{
                ASCENT_HARDWARE_BREAKPOINT;
				unmarshalState(Op_cPicked, stream);
				MARSHALSTREAM_RETURN_NULL;
			}

			case Op_PySavedStreamElement:
			{
				unmarshalState(Op_PySavedStreamElement, stream);
				uint32 index;
				if (!stream.readSizeEx(index))
					MARSHALSTREAM_RETURN_NULL;
				PyObject* obj;
				if (!mReferencedObjectsMap.GetStoredObject(index,&obj))
				{
					Log.Error("MarshalStream", "(Op_PySavedStreamElement)there seems to be a Invalid TY_REFECENCE in the stream");
					MARSHALSTREAM_RETURN_NULL;
				}

				if (obj == NULL)
				{
					Log.Error("MarshalStream", "(Op_PySavedStreamElement)GetStoredObject returned a NULL object... HUH oO wtf is happening");
					MARSHALSTREAM_RETURN_NULL;
				}

				obj->IncRef();
				MARSHALSTREAM_RETURN(obj);
			}

			/* this is fun as check the clients packet hash.... */
			case Op_PyChecksumedStream:
			{
				unmarshalState(Op_PyChecksumedStream, stream);
				uint32 clientHash;
				if (!stream.read4(clientHash))
					MARSHALSTREAM_RETURN_NULL;

				Bytef* data = &stream.content()[stream.tell()];
				uInt len = (uInt)stream.buffersize() - stream.tell();
				uint32 serverHash = adler32(1, data, len);
				if (clientHash != serverHash)
				{
					Log.Error("MarshalStream", "(Op_PyChecksumedStream) the checksum fails... client send us a 'oepsie' or he's just plain stupid");
					MARSHALSTREAM_RETURN_NULL;
				}
				else
				{
					MARSHALSTREAM_RETURN(unmarshal(stream));
				}
			}

			case Op_OldStyleClass:
				unmarshalState(Op_OldStyleClass, stream);
				MARSHALSTREAM_RETURN(ReadOldStyleClass(stream, (opcode >> 6) & 1));

			case Op_NewStyleClass:
				unmarshalState(Op_NewStyleClass, stream);
				MARSHALSTREAM_RETURN(ReadNewStyleClass(stream, (opcode >> 6) & 1));

			case Op_PyDBRow:
				unmarshalState(Op_PyDBRow, stream);
				MARSHALSTREAM_RETURN(ReadPackedRow(stream));

                /* embeded marshal stream... not a sub stream.. 
                 * "blue.MarshalStream"
                 */
			case Op_PySubStream:
				unmarshalState(Op_PySubStream, stream);
				MARSHALSTREAM_RETURN(ReadSubStream(stream));

			case Op_PyVarInteger:
				unmarshalState(Op_PyVarInteger, stream);
				MARSHALSTREAM_RETURN(ReadVarInteger(stream, (opcode >> 6) & 1));

				/* iterator sequence stop tag */
			case Op_PyStopIter:
				MARSHALSTREAM_RETURN_NULL;

			default:
			{
				//Log.Error("MarshalStream", "Invalid type tag %d, '%c' in stream.", opcode, *((char*)&opcode));
				Log.Error("MarshalStream", "Invalid type tag %d, raw 0x%X, char '%c' at:0x%X", opcode,opcode,*((char*)&opcode), stream.tell());
				MARSHALSTREAM_RETURN_NULL;
			}
		}
	}

	Log.Error("MarshalStream", "not enough data in the stream to read a additional character");
	MARSHALSTREAM_RETURN_NULL;
}

PyObject* MarshalStream::ReadBuffer( ReadStream & stream )
{
	uint32 size;
	if (!stream.readSizeEx(size))
		MARSHALSTREAM_RETURN_NULL;

	uint8* buffer;
	if (!stream.readBuffer(&buffer, size))
		MARSHALSTREAM_RETURN_NULL;

	PyString* ret = PyString_FromStringAndSize((char*)buffer, size);
	MARSHALSTREAM_RETURN(ret);
}

PyObject* MarshalStream::ReadGlobalInstance( ReadStream & stream, BOOL shared )
{
    /* Created a new Instance of a object.
    function flow...
    1. do shared object stuff....
    2. unmarshal object name ( PyString... )
    3. load a object prototype from the global object dictionary. ( macho.CallReq means... file: macho.py class:CallReq )
    4. check to make sure object is a class type.. (" global object %s should be of class type but is of %s ")
    5. register module name and stuff as global object bla bla bla...
    6. create a new raw instance of the class ( PyInstance_NewRaw ) Create a new instance of a specific class without calling its constructor.
    7. unmarshal object arguments...
    8. do some checking regarding the class attributes.... not seem to matter for us... I think its related to a function that it SHOULD have or something.... dono
    9. do some strange things regarding the blue global root class I dono maybe related to the scheduler...
    10. call the object constructor with PyObject_CallMethodObjArgs and with the object arguments of course..
    11. do some extra object refdec stuff related if the call is unsuccessful...
    12. return the new referenced class...

    // instance class structure, you see that this information is send from and to the client.
    // its logic that its done this way, because the instance contains the actual data of the object.
    // so... we need to change the way we do stuff big time hehe...

    typedef struct {
    PyObject_HEAD
    PyClassObject *in_class;	// The class object
    PyObject	  *in_dict;	// A dictionary
    PyObject	  *in_weakreflist; // List of weak references
    } PyInstanceObject;
    */

    /* we assume that what we get is always valid */
    PyString * module_name = (PyString *)unmarshal(stream);

    PyClass* class_object = sPyCallMgr.find(module_name);

    if (class_object == NULL)
    {
        module_name->DecRef();
        MARSHALSTREAM_RETURN_NULL;
    }

    if (class_object->gettype() != PyTypeClass)
    {
        module_name->DecRef();
        MARSHALSTREAM_RETURN_NULL;
    }

    /* in the retail version we now check if the call/module is in the white/black list.
     * this to filter any hacked call out of the server system.
     */

    /* PyObject* PyInstance_NewRaw(PyObject *class, PyObject *dict) 
     * Return value: New reference.
     * Create a new instance of a specific class without calling its constructor. class is the class of new object. The dict parameter will be used as the object’s __dict__; if NULL, a new dictionary will be created for the instance.
     */

    PyClass* new_instance = class_object->New();

    if (new_instance == NULL)
    {
        module_name->DecRef();
        MARSHALSTREAM_RETURN_NULL;
    }

    if (shared != FALSE)
    {
        if (mReferencedObjectsMap.StoreReferencedObject(new_instance) == -1)
        {
            module_name->DecRef();
            new_instance->DecRef();
            MARSHALSTREAM_RETURN_NULL;
        }
    }

    PyObject * bases = unmarshal(stream);

    /* for people that pay attention to detail this can really fuck things up.
     * because unless the just registered shared object ( if it is ) is removed.
     * it will end up floating around.... hehe...
     */
    if (bases == NULL)
    {
        module_name->DecRef();
        new_instance->DecRef();
        MARSHALSTREAM_RETURN_NULL;
    }

    // init the new object instance..
    new_instance->init(bases);

/*
	if (shared != FALSE)
	{
		if(mReferencedObjectsMap.StoreReferencedObject(classObj) == -1)
            ASCENT_HARDWARE_BREAKPOINT
	}

	if (classObj == NULL)
		MARSHALSTREAM_RETURN_NULL;

	
	PyTuple * bases = (PyTuple*)unmarshal(stream);

	if(!classObj->setname(module_name))
	{
		ASCENT_HARDWARE_BREAKPOINT;
		module_name->DecRef();
		classObj->DecRef();
		bases->DecRef();
		MARSHALSTREAM_RETURN_NULL;
	}

	if(!classObj->setbases(bases))
	{
		ASCENT_HARDWARE_BREAKPOINT;
		module_name->DecRef();
		classObj->DecRef();
		bases->DecRef();
		MARSHALSTREAM_RETURN_NULL;
	}*/

	//MARSHALSTREAM_RETURN(class_object);

    MARSHALSTREAM_RETURN(new_instance);
}

ASCENT_INLINE PyObject* MarshalStream::ReadInstancedClass( ReadStream & stream, BOOL shared )
{
    /*PyClass * classObj = new PyClass();

    if (shared != FALSE)
    {
        if(mReferencedObjectsMap.StoreReferencedObject(classObj) == -1)
            ASCENT_HARDWARE_BREAKPOINT
    }

    if (classObj == NULL)
        MARSHALSTREAM_RETURN_NULL;

    PyString * className = (PyString *)unmarshal(stream);

    // get the instance
    PyClass* callable_object = sPyCallMgr.find(className);

    PyTuple * bases = (PyTuple*)unmarshal(stream);

    if(!classObj->setname(className))
    {
        ASCENT_HARDWARE_BREAKPOINT;
        className->DecRef();
        classObj->DecRef();
        bases->DecRef();
        MARSHALSTREAM_RETURN_NULL;
    }

    if(!classObj->setbases(bases))
    {
        ASCENT_HARDWARE_BREAKPOINT;
        className->DecRef();
        classObj->DecRef();
        bases->DecRef();
        MARSHALSTREAM_RETURN_NULL;
    }*/

    PyString * className = (PyString *)unmarshal(stream);

    // get the instance
    PyClass* callable_object = sPyCallMgr.find(className);

    PyTuple * bases = (PyTuple*)unmarshal(stream);

    PyClass * inst_obj = callable_object->New();

    // TODO decrappy this..
    if (shared != FALSE)
    {
        if(mReferencedObjectsMap.StoreReferencedObject(inst_obj) == -1)
            ASCENT_HARDWARE_BREAKPOINT;
    }

    if (inst_obj == NULL)
        MARSHALSTREAM_RETURN_NULL;

    inst_obj->init(bases);


    /*if(!classObj->setname(className))
    {
        ASCENT_HARDWARE_BREAKPOINT;
        className->DecRef();
        classObj->DecRef();
        bases->DecRef();
        MARSHALSTREAM_RETURN_NULL;
    }*/

    /*if(!classObj->setbases(bases))
    {
        ASCENT_HARDWARE_BREAKPOINT;
        className->DecRef();
        classObj->DecRef();
        bases->DecRef();
        MARSHALSTREAM_RETURN_NULL;
    }*/

    MARSHALSTREAM_RETURN(inst_obj);
}

PyObject* MarshalStream::ReadOldStyleClass( ReadStream & stream, BOOL shared )
{
    int shared_object_index = 0;
    //ASCENT_HARDWARE_BREAKPOINT;

    if (shared != FALSE)
    {
        shared_object_index = mReferencedObjectsMap.StoreReferencedObject((PyObject*)NULL);
        if(shared_object_index == -1)
            ASCENT_HARDWARE_BREAKPOINT;
    }

	PyTuple * bases = (PyTuple *)unmarshal(stream);
	if (bases == NULL)
		MARSHALSTREAM_RETURN_NULL;

    PyObject * method = bases->GetItem(0);
    if (method == NULL)
    {
        bases->DecRef();
        MARSHALSTREAM_RETURN_NULL;
    }

    PyObject * args = bases->GetItem(1);
    if (args == NULL)
    {
        bases->DecRef();
        method->DecRef();
        MARSHALSTREAM_RETURN_NULL
    }

    // its important for this to return something...
    PyObject* call_result = PyObject_CallObject(method, args);

    if(shared)
    {
        mReferencedObjectsMap.UpdateReferencedObject(shared_object_index, call_result);        
    }

    // if the tuple hs more than 2 elements
    if (bases->size() > 2)
    {
        // implement this..
        ASCENT_HARDWARE_BREAKPOINT;
        PyObject* ext_arg = bases->GetItem(3);
        //PyObject_CallFunctionObjArgs(v13, v14, 0); // but in short its just call_result->set_states
        // call_result->set_states(ext_arg);

        /*if (call_result->set_states != NULL)
        {
            call_result->set_states(ext_arg);
        }
        else
        {
            call_result->update_dict(ext_arg);
        }
        */
    }

    // this needs to be done in a different way...... updating the class object using the class update functions...
    ReadNewObjList(stream, *(PyClass*)call_result);
    ReadNewObjDict(stream, *(PyClass*)call_result);

    MARSHALSTREAM_RETURN(call_result);
}

// C style python classes...
PyObject* MarshalStream::ReadNewStyleClass( ReadStream & stream, BOOL shared )
{
    //ASCENT_HARDWARE_BREAKPOINT;
    /* this crappy old code */
	/*PyClass * classObj = NULL;//new PyClass();

	if (shared != FALSE)
	{
		if (mReferencedObjectsMap.StoreReferencedObject(classObj) == -1)
            ASCENT_HARDWARE_BREAKPOINT;
	}

    // here new code starts 
	PyTuple * object_root = (PyTuple *)unmarshal(stream);

    PyTuple* call_root = object_root->GetItem_asPyTuple(0);
    if (call_root == NULL)
    {
        object_root->DecRef();
        MARSHALSTREAM_RETURN_NULL;
    }

    PyClass* class_instance = call_root->GetItem_asPyClass(0);
    if (class_instance == NULL)
    {
        object_root->DecRef();
        MARSHALSTREAM_RETURN_NULL;
    }*/

    /*
    PyObject* PyObject_GetAttr(PyObject *o, PyObject *attr_name)
    Return value: New reference.
    Retrieve an attribute named attr_name from object o. Returns the attribute value on success, or NULL on failure.
    This is the equivalent of the Python expression o.attr_name
    */

    /*
    PyObject* PyObject_CallObject(PyObject *method, PyObject *args)
    Return value: New reference.
    Call a callable Python object method, with arguments given by the tuple args. If no arguments are needed,
    then args may be NULL. Returns the result of the call on success, or NULL on failure. This is the equivalent of the
    Python expression apply(method, args) or method(*args).
    */

    /*
    PyObject* PyObject_CallFunctionObjArgs(PyObject *callable, ..., NULL)
        Return value: New reference.
        Call a callable Python object callable, with a variable number of PyObject* arguments. The arguments are provided
        as a variable number of parameters followed by NULL. Returns the result of the call on success, or NULL on failure.
    */

    int shared_obj_index = 0;

    /* store a NULL so we can crash when we have recursive class calls */
    if (shared != FALSE)
    {
        shared_obj_index = mReferencedObjectsMap.StoreReferencedObject((PyObject*)NULL);
        if(shared_obj_index == -1)
            ASCENT_HARDWARE_BREAKPOINT;
    }

    // here new code starts 
    PyTuple * object_root = (PyTuple *)unmarshal(stream);

    PyTuple* call_root = object_root->GetItem_asPyTuple(0);
    if (call_root == NULL)
    {
        object_root->DecRef();
        MARSHALSTREAM_RETURN_NULL;
    }

    PyClass* class_instance = call_root->GetItem_asPyClass(0);
    if (class_instance == NULL)
    {
        object_root->DecRef();
        MARSHALSTREAM_RETURN_NULL;
    }

    // do get function stuff on call_root..... bla bla bla I dono...

    // its important for this to return something...
    PyObject* call_result = PyObject_CallObject(class_instance, call_root);

    if(shared)
    {
        mReferencedObjectsMap.UpdateReferencedObject(shared_obj_index, call_result);        
    }

    // need more love here...

    ReadNewObjList(stream, *(PyClass*)call_result);
    ReadNewObjDict(stream, *(PyClass*)call_result);

    MARSHALSTREAM_RETURN(call_result);
}

PyObject* MarshalStream::ReadPackedRow( ReadStream & stream )
{
	PyPackedRow * packedRow = NULL;
	PyClass* obj1 = (PyClass*)unmarshal(stream);
	if (obj1 == NULL)
		MARSHALSTREAM_RETURN_NULL;

	packedRow = new PyPackedRow();
	packedRow->init((PyObject*)obj1);

	assert(obj1->gettype() == PyTypeClass);
		
	size_t size;
	if (!stream.readSizeEx(size))
	{
		obj1->DecRef();
		packedRow->DecRef();
		MARSHALSTREAM_RETURN_NULL;
	}

	uint8* data;
	if (!stream.readBuffer(&data, size))
	{
		obj1->DecRef();
		packedRow->DecRef();
		MARSHALSTREAM_RETURN_NULL;
	}

	size_t outsize = 0;
	uint8 *outbuff = NULL;

	size_t virtualFieldCount = 0;
	size_t fieldCount = 0;
	PyObject* rawPayload = NULL;

	if (size > 0)
	{
		size_t guessedSize = DBRowModule::GetRawFieldSizeFromHeader(obj1->getbases()->GetItem(1));

		outsize = guessedSize+20;
		size_t bufferSize = outsize;

		outbuff = (uint8*)ASCENT_MALLOC(outsize);
		assert(outbuff);

		if (!RleModule::decode(data, size, outbuff, &outsize))
		{
			SafeFree(outbuff);

			obj1->DecRef();
			Log.Error("MarshalStream", "error happened in the 'Rle' decoder");
			MARSHALSTREAM_RETURN_NULL;
		}

		/*std::vector<uint8> unfuckedup;
		
		RleModule::UnpackZeroCompressed(data, size, unfuckedup);

		//printf("unpacked mine:\n");
		//HexAsciiModule::print_hexview(stdout, outbuff, guessedSize);

		//printf("unpacked there's:\n");
		//HexAsciiModule::print_hexview(stdout, &unfuckedup[0], unfuckedup.size());

		//printf("packed source:\n");
		//HexAsciiModule::print_hexview(stdout, data, size);

		std::vector<uint8> fuckedup;
		size_t dst_len = 0;
		RleModule::PackZeroCompressed(outbuff, guessedSize, fuckedup, dst_len);

		//printf("packed repacked:\n");
		//HexAsciiModule::print_hexview(stdout, &fuckedup[0], fuckedup.size());

		if (memcmp(&fuckedup[0], data, dst_len))
		{
			printf("unpacked mine:\n");
			HexAsciiModule::print_hexview(stdout, outbuff, outsize);

			printf("unpacked there's:\n");
			HexAsciiModule::print_hexview(stdout, &unfuckedup[0], unfuckedup.size());

			printf("packed source:\n");
			HexAsciiModule::print_hexview(stdout, data, size);

			printf("packed repacked:\n");
			HexAsciiModule::print_hexview(stdout, &fuckedup[0], dst_len);
			
			ASCENT_HARDWARE_BREAKPOINT;

			guessedSize = DBRowModule::GetRawFieldSizeFromHeader(obj1->getbases()->GetItem(1));

			fuckedup.clear();
			RleModule::PackZeroCompressed(outbuff, guessedSize, fuckedup, dst_len);
		}*/

		packedRow->mRawFieldData = outbuff;
		packedRow->mRawFieldDataLen = guessedSize;

		rawPayload = DBRowModule::parseraw(*this, (PyObject*)obj1, outbuff, bufferSize, virtualFieldCount, fieldCount);
	}
	
	packedRow->setheader(obj1);
	packedRow->setRawPayLoad(rawPayload);

	/* strings related to the following code
	 * "extra virtual columns at the end"
	 * "return size of data, and number of extra columns"
	 */
	if (virtualFieldCount > 0)
	{
		size_t intergralOffset = fieldCount - virtualFieldCount;
		for (size_t i = 0; i < virtualFieldCount; i++)
		{
			PyObject* henk = unmarshal(stream);
			packedRow->addleaf(henk);
		}
	}
		
	MARSHALSTREAM_RETURN(packedRow);
}

PyObject* MarshalStream::ReadSubStream( ReadStream & stream )
{
	uint32 size = 0;
	if (!stream.readSizeEx(size))
		MARSHALSTREAM_RETURN_NULL;

	PySubStream* object = new PySubStream();
	uint8* data;
	if (!stream.readBuffer(&data, size))
	{
		object->DecRef();
		MARSHALSTREAM_RETURN_NULL;
	}

	if(!object->set(data, size))
	{
		object->DecRef();
		MARSHALSTREAM_RETURN_NULL;
	}

	MARSHALSTREAM_RETURN(object);
}

PyObject* MarshalStream::ReadVarInteger( ReadStream & stream, BOOL shared )
{
	uint32 len = 0;
	if(!stream.readSizeEx(len))
		MARSHALSTREAM_RETURN_NULL;
	
	/* we don't have big int implemented so crash if it goes crazy */
	ASCENT_ASSERT(len <= 8);
	
	PyLong* object = NULL;
	if(len == 0)
	{
		object = PyLong_FromLong(len);
	}
	else
	{
		uint8 *buffer = NULL;
		if (!stream.readBuffer(&buffer, len))
			MARSHALSTREAM_RETURN_NULL;

		object = _ByteArray_AsPyLong(buffer, len);
	}
	
	if (shared != FALSE)
	{
		if (mReferencedObjectsMap.StoreReferencedObject(object) == -1)
            ASCENT_HARDWARE_BREAKPOINT;
	}

	MARSHALSTREAM_RETURN(object);
}

PyObject* MarshalStream::ReadClassString( ReadStream & stream, BOOL shared )
{
	PyString* objectName = (PyString*)ReadBuffer(stream);

    PyClass* class_object = sPyCallMgr.find(objectName);
    assert(class_object);
	
	if (shared != FALSE)
	{
		if(mReferencedObjectsMap.StoreReferencedObject(class_object) == -1)
            ASCENT_HARDWARE_BREAKPOINT;
	}
	
	MARSHALSTREAM_RETURN(class_object);
}

bool MarshalStream::checkAndInflate( ReadStream & stream )
{
	char idChar;
	if (!stream.peek1(idChar))
	{
		Log.Error("MarshalStream", "Unable to peek 1 byte of the stream and check if its compressed");
		return false;
	}

	/* check if the packet its compressed */
	if (idChar != '~' )
	{
		if (idChar != char(0x78))
		{
			Log.Error("MarshalStream", "unknown idChar something fishy is going on..... panic");
			return false;
		}
		else
		{
			u_long  sourcelen = (u_long)stream.size();
			Bytef * source = stream.content();

			/* One of the key things of the ZLIB stuff is that we 'sometimes' don't know the size of the uncompressed data.
			 * My idea is to fix this regarding the first phase of the parsing of the data (the parser) is to go trough a
			 * couple of output buffer size. First buffer size would be 4x the initial buffer size, implying that the
			 * compression ratio is about 75%. The second buffer size would be 8x the initial buffer size, implying that the
			 * compression ratio is about 87.5%. The third and last buffer size is 16x the initial buffer size implying that
			 * the compression ratio is about 93.75%. This theory is really stupid because there is no way to actually know.
			 */

			uint32 bufferMultiplier = 4;
			u_long outBufferLen = sourcelen * bufferMultiplier;
			u_long allocatedBufferLen = outBufferLen;

			Bytef * outBuffer = (Bytef *)ASCENT_MALLOC(outBufferLen);
			
			int zlibUncompressResult = uncompress(outBuffer, &outBufferLen, source, sourcelen);
			
			if (zlibUncompressResult == Z_BUF_ERROR)
			{
				int loop_limiter = 0;
				while(zlibUncompressResult == Z_BUF_ERROR)
				{
					/* because this code is a possible fuck up, we add a check */
					if (loop_limiter++ > 100)
					{
						zlibUncompressResult = Z_MEM_ERROR;
						Log.Error("MarshalStream", "uncompress increase buffer overflow safe mechanism");
						break;
					}

					bufferMultiplier*=2;
					outBufferLen = sourcelen * bufferMultiplier;

					Log.Warning("MarshalStream","uncompress failed and we now do the increase buffer size trick: %d times. Resizing from: %u to %u", loop_limiter, outBufferLen / bufferMultiplier,outBufferLen);
					
					allocatedBufferLen = outBufferLen;

					outBuffer = (Bytef*)ASCENT_REALLOC(outBuffer, outBufferLen); // resize the output buffer
					zlibUncompressResult = uncompress(outBuffer, &outBufferLen, source, sourcelen); // and try it again
				}

				if (zlibUncompressResult != Z_OK)
				{
					Log.Error("MarshalStream", "uncompress went wrong ***PANIC***");

					free(outBuffer);
					return false;
				}
			}
			else if (zlibUncompressResult != Z_OK)
			{
				Log.Error("MarshalStream", "uncompress went wrong ***PANIC***");
				free(outBuffer);
				return false;
			}
			
			/* if we get here it usually means uncompress was successful. */
			stream.set(outBuffer, outBufferLen);

			free(outBuffer);
		}
	}
	return true;
}

bool MarshalStream::ReadNewObjList( ReadStream & stream, PyClass & obj )
{
	PyList * list = PyList_New(0);
	PyObject * listItr = NULL;
	bool healthyExit = false;
	while (true)
	{
		char thingy;
		if (stream.peek1(thingy))
		{
			if (thingy != Op_PyStopIter)
			{
				listItr = unmarshal(stream);
			}
			else
			{
				listItr = NULL;

				if(!stream.seek(1, SEEK_CUR))
				{
					Log.Error("MarshalStream", "unable to skip '-' in ReadNewObjList");
					assert(false);
				}
				else
				{
					healthyExit = true;
				}
			}
		}
		else
		{
			listItr = NULL;
			Log.Error("MarshalStream", "unable to peek for itr stop char in ReadNewObjList");
			assert(false);
		}

		if (healthyExit == true)
			break;

		if (listItr == NULL)
			return false;

		list->add(listItr);
	}

	obj.setDirList(list);

	return true;
}

bool MarshalStream::ReadNewObjDict( ReadStream & stream, PyClass & obj )
{
	PyDict * dict = PyDict_New();
	PyObject * dictItr = NULL;
	bool healthyExit = false;
	while (true)
	{
		char thingy;
		if (stream.peek1(thingy))
		{
			if (thingy != Op_PyStopIter)
			{
				dictItr = unmarshal(stream);
			}
			else
			{
				dictItr = NULL;

				if(!stream.seek(1, SEEK_CUR))
				{
					Log.Error("MarshalStream", "unable to skip '-'");
					assert(false);
				}
				else
				{
					healthyExit = true;
				}
			}
		}
		else
		{
			dictItr = NULL;
			Log.Error("MarshalStream", "unable to peek for itr stop char");
			assert(false);
		}

		if (healthyExit == true)
			break;

		if (dictItr == NULL)
			return false;

		PyObject * keyName = unmarshal(stream);

		dict->set_item(dictItr, keyName);
		keyName->DecRef();
		dictItr->DecRef();
	}

	obj.setDirDict(dict);

	return true;
}

/** this isn't finished because it needs reference index sequence numbers at the end of the packet */
bool MarshalStream::save( PyObject * object, WriteStream & stream )
{
	if (object == NULL)
		return false;

	stream.write1('~'); // first token
	uint32 refCountPlaceholder = 0;
	stream.write4(refCountPlaceholder);

	return marshal(object, stream);
}

/* helper function for me fucking up */
bool marshalString(const char* str, WriteStream & stream)
{
	size_t str_len = strlen(str);

	if (str_len == 0)
	{
		return stream.writeOpcode(Op_PyEmptyString);
	}
	else if(str_len == 1)
	{
		if (!stream.writeOpcode(Op_PyCharString))
			return false;
		return stream.write1(str[0]);
	}
	/*else if (str_len < 0xFF)
	{
		if (!stream.writeOpcode(Op_PyShortString))
			return false;

		if (!stream.write1(str_len))
			return false;
		return stream.write(str, str_len);
	}
	else
	{
		// I can't remember why and how....
		ASCENT_HARDWARE_BREAKPOINT;
	}*/
    else
    {
        if (!stream.writeOpcode(Op_PyLongString_Shared))
            return false;
        if (!stream.writeSizeEx(str_len))
            return false;
        return stream.write(str, str_len);
    }
}

//#  define unmarshalState(x, y) {sLog.String("State:"#x"\toffset:0x%X", y.tell());}

bool MarshalStream::marshal( PyObject * object, WriteStream & stream )
{
	uint8 object_type = ((PyInt*)object)->gettype();
	
	switch (object_type)
	{
	case PyTypeNone:
		{
			return stream.writeOpcode(Op_PyNone);
		} break;

	case PyTypeBool:
		{
			PyBool & b = *(PyBool *)object;
			if (b == true)
				return stream.writeOpcode(Op_PyTrue);
			else
				return stream.writeOpcode(Op_PyFalse);
		} break;

	case PyTypeLong:
		{
			int64 val = ((PyLong *)object)->GetValue();

			if (val < 0)
			{
				/* if our value is smaller than 0 meaning its a negative value we can't send it trough the optimized system so send it as long long */
				if(!stream.writeOpcode(Op_PyLongLong))
					return false;
				return stream.write8(val);
			}
			else
			{
                if (object->GetRef() > 1)
                    ASCENT_HARDWARE_BREAKPOINT;
				return WriteVarInteger(stream, object);
			}
			break;
		}
	case PyTypeInt:
		{
			int32 val = ((PyInt *)object)->GetValue();;

			if (val == -1)
			{
				return stream.writeOpcode(Op_PyMinusOne);
			}
			else if (val == 0)
			{
				return stream.writeOpcode(Op_PyZeroInteger);
			}
			else if (val == 1)
			{
				return stream.writeOpcode(Op_PyOneInteger);
			}
			else if ( val + 0x80u > 0xFF )
			{
				if ( val + 0x8000u > 0xFFFF )
				{
					if (!stream.writeOpcode(Op_PyLong))
						return false;
					return stream.write4(val);
				}
				else
				{
					if (!stream.writeOpcode(Op_PySignedShort))
						return false;
					return stream.write2(val);
				}
			}
			else
			{
				if (!stream.writeOpcode(Op_PyByte))
					return false;
				return stream.write1(val);
			}
		} break;

	case PyTypeReal:
		{
			PyFloat & number = *(PyFloat*)object;
			double val = number.GetValue();
			if (val == 0.0)
				return stream.writeOpcode(Op_PyZeroFloat);

			if (!stream.writeOpcode(Op_PyFloat))
				return false;

			return stream.write8(val);
		} break;

	case PyTypeString:
		{
            //if (object->GetRef() > 1)
              //  ASCENT_HARDWARE_BREAKPOINT;

            PyString & str = *(PyString*)object;
			size_t str_len = str.length();
			if (str_len == 0)
			{
				return stream.writeOpcode(Op_PyEmptyString);
			}
			else if(str_len == 1)
			{
				if (!stream.writeOpcode(Op_PyCharString))
					return false;
				return stream.write1(str[0]);
			}

			size_t str_index = sPyStringTable.LookupIndex(str.content());
			if (str_index != -1)
			{
				if (!stream.writeOpcode(Op_PyStringTableItem))
					return false;
				return stream.write1(str_index);
			}
			/*else if (str_len < 0xFF)
			{
				if (!stream.writeOpcode(Op_PyShortString))
					return false;

				if (!stream.write1(str_len))
					return false;
				return stream.write(str.content(), str_len);
			}*/
			else
			{
                //if (object->GetRef() > 1)
                //Op_PyLongString

                if (!stream.writeOpcode(Op_PyLongString_Shared))
                    return false;

                if (!stream.writeSizeEx(str_len))
                    return false;
                return stream.write(str.content(), str_len);
			}
		} break;

	case PyTypeUnicode:
		{
			/*
			v32 = PyUnicodeUCS2_AsUTF8String(ArgList);
			*/
			PyUnicodeUCS2 & str = *(PyUnicodeUCS2*)object;
			size_t str_len = str.length();
			if (str_len == 0)
			{
				return stream.writeOpcode(Op_PyUnicodeEmptyString);
			}
			else if(str_len == 1)
			{
				if (!stream.writeOpcode(Op_PyUnicodeCharString))
					return false;
				return stream.write2(str.content()[0]);
			}

			PyString* utf8str = (PyString*)PyUnicode_AsUTF8String(object);
			if (utf8str != NULL)
			{
				if (utf8str->length() < str_len * 2)
				{
					if (!stream.writeOpcode(Op_PyUnicodeUTF8String))
					{
						utf8str->DecRef();
						return false;
					}

					if (!stream.writeSizeEx(utf8str->length()))
					{
						utf8str->DecRef();
						return false;
					}

					if (stream.write(utf8str->content(), utf8str->length()))
					{
						utf8str->DecRef();
						return true;
					}
					else
					{
						utf8str->DecRef();
						return false;
					}
				}
			}

			/* if we got here we can't save bandwidth by sending the unicode string as UTF8 */
			if (!stream.writeOpcode(Op_PyUnicodeString))
				return false;

			if (!stream.writeSizeEx(str.length()))
				return false;

			return stream.write((char*)str.content(), str.length() * 2);
		} break;

	case PyTypeDict:
		{
            if (object->GetRef() > 1)
                ASCENT_HARDWARE_BREAKPOINT;

            PyDict & dict = *(PyDict*)object;			

			if(!stream.writeOpcode(Op_PyDict))
				return false;
			
			if (!stream.writeSizeEx(dict.size()))
				return false;

			PyDict::iterator itr = dict.begin();
			for (; itr != dict.end(); itr++)
			{
				PyDictEntry * entry = itr->second;

				/* marshal the payload */
				if(!marshal(entry->obj, stream))
					return false;

				/* marshal the key */
				if(!marshal(entry->key, stream))
					return false;
			}
			return true;
		} break;

	case PyTypeTuple:
		{
            PyTuple & tuple = *(PyTuple*)object;
            uint8 flag = 0;
            if (tuple.GetRef() > 1)
                ASCENT_HARDWARE_BREAKPOINT;


            /* if we have more references that 1 we need to check if can save data :D by using reference opcode stuff. */
            if ( tuple.GetRef() > 1 && tuple.size() != 0)
            {
                // debug this to see if it works
                ASCENT_HARDWARE_BREAKPOINT;

                uint32 ref_index = 0;
                if ( RefFindOrInsert(object, ref_index) == true )
                {
                    if (!stream.writeOpcode(Op_PySavedStreamElement))
                        return false;
                    return stream.writeSizeEx(ref_index);
                }
                else
                {
                    /* object not found and added to the index, add the notifier to the opcode so that the unmarshalling knows
                     * that the following object is a reference object
                     */
                    flag = 0x40;
                }
            }
            
            switch(tuple.size())
            {
            case 0:
                return stream.writeOpcode(Op_PyEmptyTuple);
            case 1:
                if(!stream.writeOpcode(Op_PyOneTuple | flag))
                    return false;
                break;
            case 2:
                if(!stream.writeOpcode(Op_PyTwoTuple | flag))
                    return false;
                break;
            default:
                if(!stream.writeOpcode(Op_PyTuple | flag))
                    return false;

                if(!stream.writeSizeEx(tuple.size()))
                    return false;

                break;
            }

            PyTuple::iterator itr = tuple.begin();
			for (; itr != tuple.end(); itr++)
			{
				PyChameleon* leaf = (*itr);
				if (leaf == NULL)
					return false;
				if (leaf->getPyObject() == NULL)
					return false;
				if(!marshal(leaf->getPyObject(), stream))
					return false;
			}
			return true;
		} break;

	case PyTypeList:
		{
            if (object->GetRef() > 1)
                ASCENT_HARDWARE_BREAKPOINT;

			PyList & list = *(PyList*)object;
			switch(list.size())
			{
			case 0:
				return stream.writeOpcode(Op_PyEmptyList);
			case 1:
				if(!stream.writeOpcode(Op_PyOneList))
					return false;
				break;
			default:
				if(!stream.writeOpcode(Op_PyList))
					return false;

				if(!stream.writeSizeEx(list.size()))
					return false;
				break;
			}

			for (size_t i = 0; i < list.size(); i++)
			{
				PyChameleon& leaf = list[int(i)];
				if (&leaf == NULL)
					return false;
				if (leaf.getPyObject() == NULL)
					return false;
				if(!marshal(leaf.getPyObject(), stream))
					return false;
			}
			return true;
		} break;

	case PyTypeSubStream: 
		{
			PySubStream & substream = *(PySubStream*)object;

			if(!stream.writeOpcode(Op_PySubStream))
				return false;

			if (!stream.writeSizeEx(substream.size()))
				return false;

			return stream.write((char*)substream.content(), substream.size());
		} break;

	case PyTypeClass:
		{
            uint8 flag = 0;
            if (object->GetRef() > 1)
            {
                uint32 ref_index = 0;
                if ( RefFindOrInsert(object, ref_index) == true )
                {
                    if (!stream.writeOpcode(Op_PySavedStreamElement))
                        return false;
                    return stream.writeSizeEx(ref_index);
                }
                else
                {
                    /* object not found and added to the index, add the notifier to the opcode so that the unmarshalling knows
                    * that the following object is a reference object
                    */
                    flag = 0x40;
                }
            }

            PyClass * classObject = (PyClass*)object;

            /* check if its a module... this stuff sucks bigtime */
            if (classObject->getname() != NULL &&
                classObject->getbases() == NULL &&
                classObject->getdict() == NULL &&
                classObject->getDirList() == NULL
                )
            {
                if (!stream.writeOpcode(Op_PyModule | flag))
                    return false;

                return marshal((PyObject*)classObject->getname(), stream);
            }

            if (!stream.writeOpcode(Op_PyGlobalObject | flag))
				return false;
			
			if(!marshal((PyObject*)classObject->getname(), stream))
				return false;
			if(!marshal((PyObject*)classObject->getbases(), stream))
				return false;
			return true;
		} break;

	case PyTypePackedRow: 
		{
			ASCENT_HARDWARE_BREAKPOINT;
		} break;

	case PyTypeSubStruct: 
		{
			ASCENT_HARDWARE_BREAKPOINT;
		} break;

	case PyTypeDeleted: {assert(false);} break;

	default:
		uint8 sjaak = object->gettype();
		Log.Error("MarshalStream","marshalling unhandled tag[0x%X].... sometying borked..", sjaak);
		
		Dump(stdout, object, 0);
		ASCENT_HARDWARE_BREAKPOINT;
		break;
	}

	//every case should return its own stuff regarding success or not
	ASCENT_HARDWARE_BREAKPOINT;
	return true;
}

bool MarshalStream::checkForBytecode( ReadStream & stream )
{
	uint8 byteCodeTag;
	if (!stream.peek1(byteCodeTag))
	{
		Log.Error("MarshalStream","unable to check marshal stream for byte code");
		return false;
	}

	if (byteCodeTag == 0x63 || byteCodeTag == 0xB3)
	{
		printf("dumpting python bytecode to file\n");
		char byteCodePacketName[MAX_PATH];

		/* hash the buffer so we can dump them easily */
		uint32 bytecodeHash = Utils::Hash::djb2_hash((char*)stream.content(), (int)stream.buffersize());
		snprintf(byteCodePacketName, MAX_PATH, "bytecodeStream_%u.pyc", bytecodeHash);
		FILE * fp = fopen(byteCodePacketName, "wb");
		fwrite((char*)stream.content(), stream.buffersize(), 1, fp);
		fclose(fp);
		stream.seek(0, SEEK_END); // we have read everything into the bytecode buffer.... so we now act like we have parsed the entire file
		return true;
	}
	return false;
}

ASCENT_INLINE bool MarshalStream::WriteVarInteger( WriteStream& stream, PyObject* number )
{
	/* check if something went wrong */
	if (number == NULL)
		return false;

	if (number->gettype() != PyTypeLong)
		return false;

	int64 num = ((PyLong*)number)->GetValue();
	int64 tempNum = num;
	bool negative = false;
	if (num < 0)
	{
		negative = true;
		num = -num;
	}

	/* this part checks how many bytes the number is using */
	uint8 integerSize = 0;
	uint8* num_buff = (uint8*)&num;
	for (int i = 8; i >= 1; i--)
	{
		if (num_buff[i-1] != 0)
		{
			integerSize = i;
			break;
		}
	}

	uint8* pTempNum = (uint8*)&tempNum;

	/** the theory about this piece of code: the python var integer system is fucked up...
	 * @todo Captnoord: complete this comment and improve the code.
	 */
	if (pTempNum[integerSize-1] & 0x80 && negative == false)
	{
		integerSize++;
	}
	else if (negative == true)
	{
		num_buff[integerSize-1] |= 0x80; /* @todo implement this in the read function.... argsh(*&^*&^%$ */
	}

	if(!stream.reserve(integerSize + 2)) /* reserve 2 extra because we know we have 2 additional bytes */
		return false;

	if (!stream.writeOpcode(Op_PyVarInteger))
		return false;

	if (!stream.write1(integerSize))
		return false;

	return stream.write((char*)num_buff, integerSize);
}

// undef our return null macro.
#undef MARSHALSTREAM_RETURN_NULL