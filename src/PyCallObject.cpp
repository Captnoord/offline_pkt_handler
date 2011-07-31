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
#include "machoNetAddress.h"

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
    reg("util.CachedObject",    new utilCachedObject());
    reg("util.KeyVal",          new utilKeyVal());
    reg("util.Rowset",          new utilRowset());

    /* dbutil */
    reg("dbutil.CRowset",       new dbutilCRowset());

    /* blue c api */
    reg("blue.DBRowDescriptor", new blueDBRowDescriptor());

    /* macho */
    reg("macho.CallReq",        new machoCallReq());
    reg("macho.CallRsp",        new machoCallRsp());
    reg("macho.MachoAddress",   new MachoAddress());
    reg("macho.SessionInitialStateNotification", new machoSessionInitialStateNotification());
    reg("macho.SessionChangeNotification", new machoSessionChangeNotification());
    reg("macho.PingRsp",        new machoPingRsp());
    reg("macho.PingReq",        new machoPingReq());
    reg("macho.ErrorResponse",  new machoErrorResponse());
    reg("macho.Notification",   new machoNotification());

    /* exceptions */
    reg("exceptions.GPSTransportClosed", new exceptionsGPSTransportClosed());

}

CallMgr::~CallMgr()
{
    CallMapItr itr = mCallMap.begin();
    for (; itr != mCallMap.end(); itr++)
        PyDecRef(itr->second);
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
