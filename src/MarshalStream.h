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

#ifndef _MARSHALSTREAM_H
#define _MARSHALSTREAM_H

/**
 * \class MarshalStream
 *
 * @brief this class is a wrapper for the serialization and deserialization of the eve-
 * online python marshal protocol.
 *
 * This class is a wrapper for the serialization and deserialization of the eve online
 * python marshal protocol. For the moment the deserialization is quite complete but not
 * perfect. The serialization still needs loads of love and care but eventually it will
 * get there.
 *
 * @author Captnoord
 * @date March 2009
 */
class MarshalStream
{
public:
    /** ctor & dtor */
	MarshalStream();
	~MarshalStream();

	/**
	 * \brief load loads and deserializes a marshal stream into python objects.
	 *
	 * @param[in] stream a data stream containing the serialized data.
	 * @return the deserialized object or NULL when something went wrong.
	 */
	PyObject* load(ReadStream & stream);

	/**
	 * \brief save deserializes a PyObject into a marshal stream.
	 *
	 * @param[in] object is the python object that requires to be serialized.
	 * @param[out] stream is the data stream containing the serialized object.
	 * @return true if successful and false when something went wrong.
	 */
	bool save(PyObject * object, WriteStream & stream);

	/* common packet python variable that needs to be public */
	PyBaseNone PyNone;

private:
	
	/* common packet python variables */
	PyInt PyIntZero;
	PyInt PyIntOne;
	PyInt PyIntMinOne;
	PyInt PyIntMinusOne;

	PyFloat PyFloatZero;
	PyBool	Py_TrueStruct;
	PyBool	Py_ZeroStruct;

	PyString PyStringEmpty;

	/* container to keep track of the referenced objects */
	UnmarshalReferenceMap mReferencedObjectsMap;

	/**
	 * \brief unmarshal this function deserializes the python stream.
	 *
	 * this functions parses the data to objects.
	 *
	 * @param[in] stream is the data read stream that contains the data and keeps info about read index and stream size.
	 * @return returns the deserialized PyObject and if there was a error it returns NULL.
	 */
	PyObject* unmarshal(ReadStream & stream);

	/**
	 * \brief marshal this function attempts to serialize the python objects to a data stream.
	 *
	 * this function serializes the objects to a data stream.
	 *
	 * @param[in] object is the object that needs to be serialized.
	 * @param[out] stream is the data stream that contains the serialized object.
	 * @return true if successful and false if a error has happened.
	 */
	bool marshal(PyObject * object, WriteStream & stream);

	/**
	 * @brief checkAndInflate is a temp function to handle the possibility that a packets is zipped
	 *
	 * this should really be done different.
	 *
	 * @param[in] stream is the data stream that contains the data that needs to be checked and inflated.
	 * @return true is check is successfully.. false means something went wrong...
	 */
	bool checkAndInflate( ReadStream & stream );

	/**
	 * \brief it seems that the server sends bytecode packets to every client.
	 *
	 * this function should check for the common signs and write them to a file.
	 * important to know is that this isn't normal client behavure.
	 *
	 * @param[in] stream is the data stream that contains the data that needs to be checked for bytecode streams.
	 */
	bool checkForBytecode( ReadStream & stream );

	/**
	 * \brief ReadMarshalHeader is a internal function that reads the marshal header.
	 *
	 * ReadMarshalHeader is a internal function that reads the marshal header and parses the reference
	 * object data. It also checks if the stream contains the required first tag.
	 *
	 * @param[in] stream is the data stream that contains the header that needs to be read.
	 * @return returns true if successful and false if it wasn't
	 */
	bool ReadMarshalHeader(ReadStream & stream);

	/**
	 * @brief ReadClassString reads the class id string and returns a instance of it.
	 * 
	 * ReadClassString is a function that reads the class name string and find the
	 * instance of it and returns a new instance.
	 *
	 * @param[in] stream the data stream.
	 * @param[in] shared is the object this is true, meaning it should be saved.
	 * @return the deserialized object and NULL if it failed.
	 */
	ASCENT_INLINE PyObject* ReadClassString(ReadStream & stream, BOOL shared);

	/**
	 * \brief ReadGlobalInstance reads what seems like a global instance of a object.
	 *
     * @param[in] stream the data stream.
     * @param[in] shared is the object this is true, meaning it should be saved.
	 * @return the deserialized object and NULL if it failed.
     * @todo check the 'reverence' project about handling this one.
	 */
	ASCENT_INLINE PyObject* ReadGlobalInstance(ReadStream & stream, BOOL shared);

    /**
     * \brief ReadInstancedClass reads a static instance of a python class
     *
     * @param[in] stream the data stream.
     * @param[in] shared is the object this is true, meaning it should be saved.
	 * @return the deserialized object and NULL if it failed.
     */
    ASCENT_INLINE PyObject* ReadInstancedClass(ReadStream & stream, BOOL shared);

	/**
	 * \brief ReadReducedClass reads reduced python class from the data stream and
	 * uses it to initiate a instance of that class.
	 *
     * @param[in] stream the data stream.
     * @param[in] shared is the object this is true, meaning it should be saved.
	 * @return the deserialized object and NULL if it failed.
	 */
	ASCENT_INLINE PyObject* ReadReducedClass(ReadStream & stream, BOOL shared);

	/**
	 * \brief ReadNewStyleClass reads a new style python class and uses it to
	 * initiate a instance of that class.
	 *
     * @param[in] stream the data stream.
     * @param[in] shared is the object this is true, meaning it should be saved.
	 * @return the deserialized object and NULL if it failed.
	 */
	ASCENT_INLINE PyObject* ReadNewStyleClass(ReadStream & stream, BOOL shared);

	/**
	 * \brief ReadPackedRow reads a packed db row from the data stream.
	 *
     * @param[in] stream the data stream.
	 * @return the deserialized object and NULL if it failed.
	 */
	ASCENT_INLINE PyObject* ReadPackedRow(ReadStream & stream);

	/**
	 * \brief ReadPyStream reads a embeded marshal stream from the data stream.
	 *
     * @param[in] stream the data stream.
	 * @return the deserialized object and NULL if it failed.
	 */
	ASCENT_INLINE PyObject* ReadPyStream(ReadStream & stream);

	/**
	 * \brief ReadVarInteger reads a variable size integer from the data stream.
	 *
     * @param[in] stream the data stream.
     * @param[in] shared is the object this is true, meaning it should be saved.
	 * @return the deserialized object and NULL if it failed.
	 */
	ASCENT_INLINE PyObject* ReadVarInteger(ReadStream & stream, BOOL shared);

	/**
	 * \brief ReadBuffer reads the SizeEx chunk of the stream and reads the string accordingly.
	 *
     * @param[in] stream the data stream.
	 * @return a PyString pointer if successful and NULL if it wasn't.
	 */
	ASCENT_INLINE PyObject* ReadBuffer(ReadStream & stream);

	/**
	 * \brief ReadNewObjDict reads all the PyClass list entries from the stream.
	 *
     * @param[in] stream the data stream.
     * @param[out] obj the object where the list entries are stored in.
	 * @return the deserialized object and NULL if it failed.
	 */
	ASCENT_INLINE bool ReadNewObjList(ReadStream & stream, PyClass & obj);

	/**
	 * \brief ReadNewObjDict reads all the PyClass dict entries from the stream.
	 *
     * @param[in] stream the data stream.
     * @param[out] obj the object where the dict entries are stored in.
	 * @return the deserialized object and NULL if it failed.
	 */
	ASCENT_INLINE bool ReadNewObjDict(ReadStream & stream, PyClass & obj);


	/**
	 * \brief WriteVarInteger is a wrapper function to write a variable integer to the a stream.
	 *
	 * WriteVarInteger is a wrapper function to write a variable integer to the a stream.
	 *
	 * @param[in] stream is the buffer to be written to.
	 * @param[in] number is the PyLong object that needs to be written to the stream.
	 * @return true if successful and false if a error has happened.
	 */
	ASCENT_INLINE bool WriteVarInteger(WriteStream& stream, PyObject* number);

    /* ReferenceIndexMap
     * this stores and keeps track of shared objects within a marshal stream.
     */
    typedef std::tr1::unordered_map<uint32, uint32> MARSHAL_REF_MAP;
    typedef MARSHAL_REF_MAP::iterator RefMapItr;

    MARSHAL_REF_MAP mMarshalReferenceMap;

    /** object reference index counter */
    uint32 mMarshalReferenceCounter;

    /** this function checks if a object is already present in the reference map
     * if its not the object is added and index is set. Otherwise index is set to -1
     * This function returns true when a object is found and index is set correctly.
     * This function returns false when a object is not found and is added to the index.
     */
    bool RefFindOrInsert(PyObject* obj, uint32 & index)
    {
        assert(obj);
        //obj->hash()
        uint32 obj_hash = PyObject_Hash(obj);
        RefMapItr itr = mMarshalReferenceMap.find(obj_hash);
        
        /* if we haven't found the object is needs to be added to the map */
        if (itr == mMarshalReferenceMap.end())
        {
            mMarshalReferenceCounter++; // create a new reference index number.
            mMarshalReferenceMap.insert(std::make_pair(obj_hash, mMarshalReferenceCounter));

            /* we return -1 here because we have inserted a new shared object into the map.
             * -1 means that we don't need todo special things with it.
             */
            index = -1;

            return false;
        }
        else
        {
            uint32 ref_index = itr->second;
            index = ref_index;
            return true;
        }
        /* unreached */
    }
};

#endif //_MARSHALSTREAM_H
