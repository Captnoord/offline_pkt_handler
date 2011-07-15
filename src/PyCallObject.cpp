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
