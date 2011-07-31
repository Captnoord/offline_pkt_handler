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

#include "machoNetAddress.h"

MachoAddress::MachoAddress() : PyClass( "macho.MachoAddress" ) {}

MachoAddress::~MachoAddress()
{
    printf("destructor");
}

MachoAddress* MachoAddress::New()
{
    return new MachoAddress();
}

int MachoAddress::UpdateDict( PyObject* bases )
{
    return -1;
}

uint32 MachoAddress::get_addr_type()
{
    PyObject* address_type = mDict->get_item("addressType");
    return PyNumberGetValue(address_type);
}

PyTuple* MachoAddress::GetState()
{
    PyTuple* state = NULL;

    // this one is a bit crap.... we need to increase ref count?
    // maybe we need todo this if we do MT with shared objects between threads ( which is killing bleh )
    PyString& addressType = *(PyString*)mDict->get_item("addressType");
    if (addressType == "C")
    {
        state = new PyTuple(4);
        state->set_item(0, mDict->get_item("addressType"));
        state->set_item(1, mDict->get_item("clientID"));
        state->set_item(2, mDict->get_item("callID"));
        state->set_item(3, mDict->get_item("service"));
    }
    else if (addressType == "B")
    {
        state = new PyTuple(4);
        state->set_item(0, mDict->get_item("addressType"));
        state->set_item(1, mDict->get_item("broadcastID"));
        state->set_item(2, mDict->get_item("narrowcast"));
        state->set_item(3, mDict->get_item("idtype"));
    }
    else if (addressType == "A")
    {
        state = new PyTuple(3);
        state->set_item(0, mDict->get_item("addressType"));
        state->set_item(1, mDict->get_item("service"));
        state->set_item(2, mDict->get_item("callID"));
    }
    else if (addressType == "N")
    {
        state = new PyTuple(4);
        state->set_item(0, mDict->get_item("addressType"));
        state->set_item(1, mDict->get_item("nodeID"));
        state->set_item(2, mDict->get_item("service"));
        state->set_item(3, mDict->get_item("callID"));
    }
    else
    {
        // make sure it crashes...
        ASCENT_HARDWARE_BREAKPOINT;
    }

    return state;
}

bool MachoAddress::init( PyObject* state )
{
    if (!PyTuple_Check(state))
        return false;

    PyTuple * pState = (PyTuple *)state;

    int addressType = pState->GetItem_asInt(0);
    if (addressType == ADDRESS_TYPE_CLIENT)
    {
        mDict->set_item("addressType", pState->get_item(0));
        mDict->set_item("clientID", pState->get_item(1));
        mDict->set_item("callID", pState->get_item(2));
        mDict->set_item("service", pState->get_item(3));
    }
    else if (addressType == ADDRESS_TYPE_BROADCAST)
    {
        mDict->set_item("addressType", pState->get_item(0));
        mDict->set_item("broadcastID", pState->get_item(1));
        mDict->set_item("narrowcast", pState->get_item(2));
        mDict->set_item("idtype", pState->get_item(3));
    }
    else if (addressType == ADDRESS_TYPE_ANY)
    {
        mDict->set_item("addressType", pState->get_item(0));
        mDict->set_item("service", pState->get_item(1));
        mDict->set_item("callID", pState->get_item(2));
    }
    else if (addressType == ADDRESS_TYPE_NODE)
    {
        mDict->set_item("addressType", pState->get_item(0));
        mDict->set_item("nodeID", pState->get_item(1));
        mDict->set_item("service", pState->get_item(2));
        mDict->set_item("callID", pState->get_item(3));
    }
    else
    {
        ASCENT_HARDWARE_BREAKPOINT;
        return false;
    }
    return true;
}

bool MachoAddress::repr( FILE* fp )
{
    PyInt* addressType = (PyInt*)mDict->get_item("addressType");

    if (*addressType == ADDRESS_TYPE_CLIENT)
    {
        PyObject* _clientID =  mDict->get_item("clientID");
        PyLong* callID =     (PyLong*)mDict->get_item("callID");
        PyString* service = (PyString*)mDict->get_item("service");

        uint64 clientID = PyNumberGetValue(_clientID);

        fprintf(fp, "Address::Client(");

        if ( PyNone_Check(_clientID) )
            fprintf(fp, "clientID=\"None\",");
        else
            fprintf(fp, "clientID=\"%u\",", clientID);

        if ( PyNone_Check(callID) )
            fprintf(fp, "callID=\"None\",");
        else
            fprintf(fp, "callID=\"%u\",", callID->get_value());

        if ( !service || PyNone_Check(service) )
            fprintf(fp, "service=\"None\")");
        else
            fprintf(fp, "service=\"%s\")", service->content());

        return true;
    }
    else if (*addressType == ADDRESS_TYPE_NODE)
    {
        PyInt* nodeID =   (PyInt*)mDict->get_item("nodeID");
        PyLong* callID =     (PyLong*)mDict->get_item("callID");
        PyString* service = (PyString*)mDict->get_item("service");

        assert(( PyInt_Check(nodeID) || PyNone_Check(nodeID)) && PyLong_Check(callID) || PyNone_Check(callID));

        fprintf(fp, "Address::Node(");

        if ( PyNone_Check(callID) )
            fprintf(fp, "nodeID=\"None\",");
        else
            fprintf(fp, "nodeID=\"%u\",", nodeID->get_value());

        if ( !service || PyNone_Check(service) )
            fprintf(fp, "service=\"None\",");
        else
            fprintf(fp, "service=\"%s\",", service->content());

        if ( PyNone_Check(callID) )
            fprintf(fp, "callID=\"None\")");
        else
            fprintf(fp, "callID=\"%u\")", callID->get_value());

        return true;
    }
    else if (*addressType == ADDRESS_TYPE_ANY)
    {
        PyLong* callID =     (PyLong*)mDict->get_item("callID");
        PyString* service = (PyString*)mDict->get_item("service");

        assert(PyLong_Check(callID) || PyNone_Check(callID));

        fprintf(fp, "Address::Any(");

        if ( !service || PyNone_Check(service) )
            fprintf(fp, "service=\"None\",");
        else
            fprintf(fp, "service=\"%s\",", service->content());

        if ( PyNone_Check(callID) )
            fprintf(fp, "callID=\"None\")");
        else
            fprintf(fp, "callID=\"%u\")", callID->get_value());

        return true;
    }
    else if (*addressType == ADDRESS_TYPE_BROADCAST)
    {
        /* I haven't seem this one so I am unable to test */
        //ASCENT_HARDWARE_BREAKPOINT;

        PyString* broadcastID =  (PyString*)mDict->get_item("broadcastID");
        PyObject* narrowcast = mDict->get_item("narrowcast");
        PyString* idtype =     (PyString*)mDict->get_item("idtype");

        //assert(PyString_Check(idtype) && PyString_Check(broadcastID));

        fprintf(fp, "Address::BroadCast(");

        if ( PyNone_Check(broadcastID) )
            fprintf(fp, "broadcastID=\"None\",");
        else
            fprintf(fp, "broadcastID=\"%s\",", broadcastID->content());

        if ( PyNone_Check(narrowcast) )
            fprintf(fp, "narrowcast=\"None\",");
        else
        {
            /* temp hack.. */
            PyList* list = (PyList*)narrowcast;
            if (list->size() != 0)
                Dump(fp, list, -1, true);
            else
                fprintf(fp,"None,");

            //fprintf(fp, "narrowcast=\"something to narrowcast... need to check\",");
            //fprintf(fp, "narrowcast=\"%s\")", strx(narrowcast));
        }

        if ( PyNone_Check(idtype) )
            fprintf(fp, "idtype=\"None\")");
        else
            fprintf(fp, "idtype=\"%s\")", idtype->content());

        return true;
    }
    else
    {
        fprintf(fp, "Address::Undefined");
        return false;
    }
}

int MachoAddress::RoutesTo( PyObject *otherAddress, PyObject* fromAddress /*= NULL*/ )
{
    ASCENT_HARDWARE_BREAKPOINT;
    return 0;
}