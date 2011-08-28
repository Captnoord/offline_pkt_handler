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

/** MachoAddress
 * @note move to its own file
    
    -A macho client tuple address       4 elements
    -A macho broadcast tuple address    4 elements
    -A macho any tuple address          3 elements
    -A macho node tuple address         4 elements

    For all address types addressType is the first element.

    -Macho client
        *addressType = theTuple[0]
        *clientID = theTuple[1]
        *callID = theTuple[2]
        *service = theTuple[3]
    -Macho broadcast
        *addressType = theTuple[0]
        *broadcastID = theTuple[1]
        *narrowcast = theTuple[2]
        *idtype = theTuple[3]
    -Macho any
        *addressType = theTuple[0]
        *service = theTuple[1]
        *callID = theTuple[2]
    -Macho node
        *addressType = theTuple[0]
        *nodeID = theTuple[1]
        *service = theTuple[2]
        *callID = theTuple[3]
*/

#ifndef machoNetAddress_h__
#define machoNetAddress_h__

#include "Python.h"

#ifdef ENABLE_PACKED_CLASSES
#  pragma pack(push,1)
#endif

enum ADDRESS_TYPES {
    ADDRESS_TYPE_NODE = 1,
    ADDRESS_TYPE_CLIENT = 2,
    ADDRESS_TYPE_BROADCAST = 4,
    ADDRESS_TYPE_ANY = 8,
};

class MachoAddress : public PyClass
{
public:
    MachoAddress();

    ~MachoAddress();

    MachoAddress* New();

    /* this is a stub that needs to be implemented */
    int UpdateDict(PyObject* bases);

    uint64 get_addr_type();

    // bleh this code causes the object to be shared.... we need to make set_item increase the object ref... so this gets a bit cleaner..

    // serialize this class
    PyTuple* getstate();

    bool init(PyObject* state);

    bool setstate(PyObject* state)
    {
        sLog.Warning("util.row", " stub setstate called");
        return true;
    }

    int RoutesTo( PyObject *otherAddress, PyObject* fromAddress = NULL);

    bool repr( FILE* fp );
};

#ifdef ENABLE_PACKED_CLASSES
#  pragma pack(pop)
#endif

#endif // machoNetAddress_h__
