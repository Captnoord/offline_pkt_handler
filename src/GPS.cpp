
#include "ascent.h"
#include "Python.h"
#include "GPS.h"

exceptions_GPSTransportClosed::exceptions_GPSTransportClosed() : PyClass("exceptions.GPSTransportClosed")
{

}

exceptions_GPSTransportClosed::~exceptions_GPSTransportClosed()
{

}

exceptions_GPSTransportClosed* exceptions_GPSTransportClosed::New()
{
    return new exceptions_GPSTransportClosed();
}

bool exceptions_GPSTransportClosed::init( PyObject* state )
{
    if (!PyTuple_Check(state))
        return false;

    PyTuple * pState = (PyTuple*)state;

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

PyTuple* exceptions_GPSTransportClosed::GetState()
{
    return NULL;
}

bool exceptions_GPSTransportClosed::repr( FILE* fp )
{
    fprintf(fp, "dbutil_CRowset needs some dumping love\n");
    return true;
}