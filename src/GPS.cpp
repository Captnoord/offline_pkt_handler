
#include "ascent.h"
#include "Python.h"
#include "GPS.h"

exceptionsGPSTransportClosed::exceptionsGPSTransportClosed() : PyClass("exceptions.GPSTransportClosed")
{

}

exceptionsGPSTransportClosed::~exceptionsGPSTransportClosed()
{

}

exceptionsGPSTransportClosed* exceptionsGPSTransportClosed::New()
{
    return new exceptionsGPSTransportClosed();
}

bool exceptionsGPSTransportClosed::init( PyObject* state )
{
    sLog.Warning("util.row", "init stubb called");
    return false;

    if (!PyTuple_Check(state))
        return false;

    //PyTuple * pState = (PyTuple*)state;

    // TODO: add item count check...

    /*mDict->set_item("reason", pState->GetItem(0));
    mDict->set_item("reasonCode", pState->GetItem(1));
    mDict->set_item("reasonArgs", pState->GetItem(2));
    */

    /*args = {'reason': getattr(self, 'reason', None),
    'reasonCode': getattr(self, 'reasonCode', None),
    'reasonArgs': getattr(self, 'reasonArgs', None),
    'exception': self}*/
    return true;
}

PyTuple* exceptionsGPSTransportClosed::getstate()
{
    sLog.Warning("utilRowset", "stub getstate called");
    return NULL;
}

bool exceptionsGPSTransportClosed::repr( FILE* fp )
{
    fprintf(fp, "dbutil_CRowset needs some dumping love\n");
    return true;
}