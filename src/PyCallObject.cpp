#include "ascent.h"
#include "PyObjects.h"
#include "PyString.h"
#include "PyCallObject.h"

#include "machoNetPacket.h"

createFileSingleton(CallMgr);
CallMgr::CallMgr()
{
    // we now register the proto types...
    reg("util.Row", new util_Row());

    reg("macho.CallReq", new macho_CallReq());
    reg("macho.CallRsp", new macho_CallRsp());
    reg("macho.MachoAddress", new macho_MachoAddress());
    reg("macho.SessionInitialStateNotification", new macho_SessionInitialStateNotification());
    reg("macho.SessionChangeNotification", new macho_SessionChangeNotification());
    reg("macho.PingRsp", new macho_PingRsp());
    reg("macho.PingReq", new macho_PingReq());
    reg("macho.ErrorResponse", new macho_ErrorResponse());
    reg("macho.Notification", new macho_Notification());
}

CallMgr::~CallMgr()
{
    CallMapItr itr = mCallMap.begin();
    for (; itr != mCallMap.end(); itr++)
    {
        itr->second->DecRef();
    }
}

bool CallMgr::reg( const char* name, PyClass* module )
{
    mCallMap.insert(std::make_pair(name, module));
    return true;
}

PyClass* CallMgr::find( const char* module )
{
    CallMapItr itr = mCallMap.find(module);
    if (itr != mCallMap.end())
    {
        printf("found module: %s\n", module);
        return itr->second;
    }
    else
    {
        printf("unable to find module: %s\n", module);
        ASCENT_HARDWARE_BREAKPOINT;
        return NULL;
    }
    // debug shit    
}

PyClass* CallMgr::find( PyString* module )
{
    return find(module->content());
}
