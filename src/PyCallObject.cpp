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

#include "ascent.h"
#include "PyObjects.h"
#include "PyString.h"
#include "PyCallObject.h"

#include "machoNetPacket.h"
#include "objectCachingUtil.h"
#include "rowset.h"
#include "cRowSet.h"
#include "blue_DBRowDescriptor.h"
#include "GPS.h"

createFileSingleton( CallMgr );

CallMgr::CallMgr()
{
    /* we now register the class prototypes */
    
    /* utils */
    reg("util.Row",             new util_Row());
    reg("util.CachedObject",    new util_CachedObject());
    reg("util.KeyVal",          new util_KeyVal());
    reg("util.Rowset",          new util_Rowset());

    /* dbutil */
    reg("dbutil.CRowset",       new dbutil_CRowset());

    /* blue c api */
    reg("blue.DBRowDescriptor", new blue_DBRowDescriptor());

    /* macho */
    reg("macho.CallReq",        new macho_CallReq());
    reg("macho.CallRsp",        new macho_CallRsp());
    reg("macho.MachoAddress",   new macho_MachoAddress());
    reg("macho.SessionInitialStateNotification", new macho_SessionInitialStateNotification());
    reg("macho.SessionChangeNotification", new macho_SessionChangeNotification());
    reg("macho.PingRsp",        new macho_PingRsp());
    reg("macho.PingReq",        new macho_PingReq());
    reg("macho.ErrorResponse",  new macho_ErrorResponse());
    reg("macho.Notification",   new macho_Notification());

    /* exceptions */
    reg("exceptions.GPSTransportClosed", new exceptions_GPSTransportClosed());

}

CallMgr::~CallMgr()
{
    CallMapItr itr = mCallMap.begin();
    for (; itr != mCallMap.end(); itr++) {

        itr->second->DecRef();

    }
}

bool CallMgr::reg( const char* guid, PyClass* module )
{
    mCallMap.insert(std::make_pair(guid, module));
    return true;
}

PyClass* CallMgr::find( const char* module )
{
    /* stupid debug checks */
    /*
    if (!strcmp("dbutil.CRowset", module))
        ASCENT_HARDWARE_BREAKPOINT;

    if (!strcmp("exceptions.GPSTransportClosed", module))
        ASCENT_HARDWARE_BREAKPOINT;
    */

    CallMapItr itr = mCallMap.find(module);
    if (itr != mCallMap.end()) {

        //printf("found module: %s\n", module);
        return itr->second;

    } else {

        printf("unable to find module: %s\n", module);
        ASCENT_HARDWARE_BREAKPOINT;
        return NULL;
    }
}

PyClass* CallMgr::find( PyString* module )
{
    return find(module->content());
}
