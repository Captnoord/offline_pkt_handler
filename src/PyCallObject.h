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

#ifndef PYCALLOBJECT_H
#define PYCALLOBJECT_H

#include "PyObjectDumper.h"
#include "Python.h"

#ifdef ENABLE_PACKED_CLASSES
#  pragma pack(push,1)
#endif


/*  PyClass
    one of the way of communication is exchange states...
    so lets say we have a generic class which has variable called 'itemID'.
    we can use getstate and setstate to serialize and deserialize the class
    state.
    Often this is done by using tuple's containing only the data objects from the
    PyClass.
*/

/* move this to its own file */
class util_Row : public PyClass
{
public:
    util_Row() : PyClass( "util.Row" ) {}
    ~util_Row()
    {        
    }

    util_Row* New()
    {
        return new util_Row();
    }

    /* this is a stub that needs to be implemented */
    int UpdateDict(PyObject* bases)
    {
        return -1;
    }

    /* this is a stub that needs to be implemented */
    bool init(PyObject* state)
    {
        sLog.Warning("util.row", "init stubb called");
        return false;
    }

    bool setstate(PyObject* state)
    {
        sLog.Warning("util.row", "stub setstate called");
        return true;
    }

    /* this is a stub that needs to be implemented */
    PyTuple* getstate()
    {
        sLog.Warning("utilRowset", "stub getstate called");
        return NULL;
    }

    bool repr( FILE* fp )
    {
        fprintf(fp, "dbutil_CRowset needs some dumping love\n");
        return true;
    }
};

#ifdef ENABLE_PACKED_CLASSES
#  pragma pack(pop)
#endif



class CallMgr : public Singleton<CallMgr>
{
public:
    CallMgr();
    ~CallMgr();

    bool reg(const char* name, PyClass* module);

    PyClass* find(const char* module);
    PyClass* find(PyString* module);

private:
    typedef std::map<std::string, PyClass*> CallMap;
    typedef CallMap::iterator CallMapItr;

    CallMap mCallMap;
};

#endif // PYCALLOBJECT_H

#define sPyCallMgr CallMgr::getSingleton()
