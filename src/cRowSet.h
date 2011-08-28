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
#include "Python.h"
#include "PyObjectDumper.h"

#ifndef cRowSet_h__
#define cRowSet_h__

#ifdef ENABLE_PACKED_CLASSES
#  pragma pack(push,1)
#endif

class dbutilCRowset : public PyClass
{
protected:
public:
    dbutilCRowset() : PyClass( "dbutil.CRowset" ), mList( NULL )
    {
        if (mList != NULL)
            PyDecRef(mList);
        mList = new PyList();
    }

    // this of course will never be called
    ~dbutilCRowset()
    {
        PyDecRef(mList);
    };

    dbutilCRowset* New()
    {
        return new dbutilCRowset();
    }

    // comments: format guessed from compiled scripts
    bool init(PyObject* state)
    {
        PyTuple * pTuple = (PyTuple *)state;

        mList->init(pTuple->get_item(1));
        mDict->set_item("header", pTuple->get_item(0));


        /*if (!PyTuple_Check(state))
        return false;
        PyTuple * pTuple = (PyTuple*)state;

        mDict->set_item("header", pTuple->GetItem(0));
        mDict->set_item("lines", pTuple->GetItem(1));
        mDict->set_item("RowClass", pTuple->GetItem(2));*/

        //if (state->gettype() != PyTypeDict)
          //  return false;

        // TODO check if this has any data in it... it should be empty
        //PyDecRef(mDict);

        // replace the current dict with that one to init...
        //mDict = (PyDict*)state; state->IncRef();

        return true;
    }

    bool setstate(PyObject* state)
    {
        sLog.Warning("util.row", "stub setstate called");
        return true;
    }

    PyTuple* getstate()
    {
        sLog.Warning("utilRowset", "stub getstate called");
        return NULL;
    };

    bool repr( FILE* fp )
    {
        fprintf(fp, "dbutilCRowset needs some dumping love\n");
        return true;
    }


    /* this object derives from a PyList.... we hack it this way for now... */
    PyList * mList;
};

#ifdef ENABLE_PACKED_CLASSES
#  pragma pack(pop)
#endif

#endif // cRowSet_h__

