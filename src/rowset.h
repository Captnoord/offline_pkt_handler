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

#include "PyObjectDumper.h"
#include "Python.h"

#ifdef ENABLE_PACKED_CLASSES
#  pragma pack(push,1)
#endif

class util_KeyVal : public PyClass
{
protected:
public:
    util_KeyVal() : PyClass( "util.KeyVal" ) {}

    ~util_KeyVal(){};

    util_KeyVal* New()
    {
        return new util_KeyVal();
    }

    /* comments: format guessed from compiled scripts */
    bool init(PyObject* state)
    {
        if (state->GetType() != PyTypeDict)
            return false;
        
        // TODO check if this has any data in it... it should be empty
        PyDecRef(mDict);

        // replace the current dict with that one to init...
        mDict = (PyDict*)state;
        PyIncRef(state);
        return true;
    }

    /* this is a stub that needs to be implemented */
    PyTuple* GetState()
    {
        return NULL;
    };

    bool repr( FILE* fp )
    {
        fprintf(fp, "dbutil_CRowset needs some dumping love\n");
        return true;
    }

};

class util_Rowset : public PyClass
{
protected:
public:
    util_Rowset() : PyClass( "util.Rowset" ) {}

    ~util_Rowset(){};

    util_Rowset* New()
    {
        return new util_Rowset();
    }

    /* comments: format guessed from compiled scripts */
    bool init(PyObject* state)
    {
        /*if (!PyTuple_Check(state))
            return false;
        PyTuple * pTuple = (PyTuple*)state;

        mDict->set_item("header", pTuple->GetItem(0));
        mDict->set_item("lines", pTuple->GetItem(1));
        mDict->set_item("RowClass", pTuple->GetItem(2));*/

        if (!PyDict_Check(state))
            return false;

        // TODO check if this has any data in it... it should be empty
        PyDecRef(mDict);

        // replace the current dict with that one to init...
        mDict = (PyDict*)state;
        PyIncRef(state);

        return true;
    }

    /* this is a stub that needs to be implemented */
    PyTuple* GetState()
    {
        return NULL;
    }

    bool repr( FILE* fp )
    {
        fprintf(fp, "dbutil_CRowset needs some dumping love\n");
        return true;
    }


    // todo implement 'Row'
    //RowClass = Row
};

#ifdef ENABLE_PACKED_CLASSES
#  pragma pack(pop)
#endif
