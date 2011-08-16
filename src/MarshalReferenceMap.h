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

#ifndef UNMARSHAL_REFERENCE_MAP_H
#define UNMARSHAL_REFERENCE_MAP_H

#include "Common.h"

/**
 * MarshalReferenceMap.h only implement the UnmarshalReferenceMap, the Marshal reference tech
 * isn't researched/implemented for the moment. a cpp file should be created from the code in
 * this header but for the moment this will do.
 * @note because we don't own the pointer / own the memory of the referenced objects. Its unclear when the referred object is deleted so
 * we have to either keep the Object here ( own the pointer ) or make sure that the referenced object know he is a referenced object so
 * it isn't accidentally deleted.
 */

/**
 * \class UnmarshalReferenceMap
 *
 * @brief 
 *
 * the main idea is that if 1 objects has multiple references within a MarshalStream, there send like references.
 *
 * @author Captnoord
 * @date December 2008
 */
class UnmarshalReferenceMap
{
public:

	UnmarshalReferenceMap() : mExpectedObjectsCount(0), mStoredObjectCount(0), mStoreObjectIndex(0), mReferenceObjects(NULL) {}

	/**
	 * Constructor that sets the expected object count of referenced objects.
	 * @param objectCount is the number of expected referenced objects within the marshal stream, its also known in the client as as MapCount.
	 */
	UnmarshalReferenceMap(const uint32 objectCount) : mExpectedObjectsCount(objectCount), mStoredObjectCount(0), mStoreObjectIndex(0), mReferenceObjects(NULL)
	{
        ASCENT_HARDWARE_BREAKPOINT;
		assert(false);
	}

	~UnmarshalReferenceMap()
	{
        if (!mReferenceObjects.empty()) {
		    for (uint32 i = 0; i < mExpectedObjectsCount; i++) {
			    PySafeDecRef(mReferenceObjects[i]);
		    }
        }

		mReferenceObjects.clear();
	}

	/**
	 * Get a stored referenced Object.
	 * @param location is the index number of the referenced object.
	 * @return A referenced PyRep base Object.
	 */
	bool GetStoredObject(uint32 location, PyObject** object)
	{
		if (location == 0 || (location-1) > mStoredObjectCount )
			return false;

        if (mReferenceObjects.size() < location)
            return false;

		*object = mReferenceObjects[location-1];
        if (*object == NULL)
            return false;

		return true;
	}

	/**
	 * Stores a referenced Object.
	 * @param object is the object that is marked as a object that has many references.
     * @return returns the object index number.
	 */
	template<typename T>
	int StoreReferencedObject(T* object)
	{
        if (object == NULL)
            return -1;

        if (mExpectedObjectsCount == 0)
            return -1;

		if (mStoreObjectIndex > mExpectedObjectsCount)
			return -1;

		uint32 objectloc = mOrderMap[mStoreObjectIndex] - 1;
		assert(objectloc >= 0);

		//printf("store referenced object: %d to %d\n", mStoreObjectIndex, objectloc);

		mReferenceObjects[objectloc] = (PyObject*)object;
		mStoreObjectIndex++;

        // makes sure we increase the ref counter..., its possible that we store a NULL pointer...
        if (object != NULL)
            PyIncRef((PyObject*)object);

		mStoredObjectCount = mStoreObjectIndex;
		return (int)objectloc;
	}

    template<typename T>
    bool UpdateReferencedObject(int index, T* object)
    {
        //ASCENT_HARDWARE_BREAKPOINT;

        assert(object);

        PyObject** shared_obj_ptr = &mReferenceObjects[index];
        assert(!*shared_obj_ptr); // for now... this should be NULL.... if its not crash..

        *shared_obj_ptr = (PyObject*)object;

        PyIncRef((PyObject*)object);

        return true;
    }



	/**
	 * Get the current stored objects.
	 * @return A unsigned integer that represents the amount of referenced Objects stored.
	 */
	uint32 GetStoredCount()
	{
		return mStoredObjectCount;
	}

	/**
	 * Get the expected referenced object count.
	 * @return A unsigned integer that represents the expected object count.
	 */
	uint32 GetMaxStoredCount()
	{
		return mExpectedObjectsCount;
	}

	bool SetOrderMapSize(int size)
	{
        if (size > 0x1000)
            return false;
		mOrderMap.resize(size+1);
        return true;
	}

	void SetObjectOrder(int index, int order)
	{
		mOrderMap[index] = order;
	}

	/**
	 * @brief Set the amount of shared objects
	 *
	 * 
	 *
	 * @param[in]
	 */
	bool SetSharedObjectCount(uint32 sharedObjectCount)
	{
        if (sharedObjectCount > 0x1000)
            return false;
		// this shouldn't happen and if it does.. this prop will generate a crash...
		//if(mReferenceObjects != NULL)
		//	SafeFree(mReferenceObjects)
			//SafeDeleteArray(mReferenceObjects);

		assert(mReferenceObjects.empty());
		mReferenceObjects.resize(sharedObjectCount+1);

		//mReferenceObjects = new PyObject*[expectedObjectsCount+1];
		//mReferenceObjects = (PyObject**)ASCENT_MALLOC(sizeof(PyObject*) * (expectedObjectsCount+1));
		mExpectedObjectsCount = sharedObjectCount;
        return true;
	}

protected:
	/** 
	 * keeps track of the amount of objects that are actually stored
	 */
	uint32 mStoredObjectCount;

	/**
	 * keeps track of the store index
	 */
	uint32 mStoreObjectIndex;

	/**
	 * max amount of objects that are referenced objects in this stream
	 * this value is supplied by the client (regarding the 'unmarshal' process).
	 */
	uint32 mExpectedObjectsCount;

	/**
	 * pointer container to keep track of the pointers...
	 * and of course: "we do not own the pointers in this list"
	 */
	//PyObject** mReferenceObjects;
	std::vector<PyObject*> mReferenceObjects;
	std::vector<int> mOrderMap;
};

#endif//UNMARSHAL_REFERENCE_MAP_H
