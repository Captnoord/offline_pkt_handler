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
#include "Python.h"

#include "machoNetPacket.h"
#include "machoNetAddress.h"

MachoPacket::MachoPacket( const char* derived_name ) : PyClass(derived_name) {}
MachoPacket::~MachoPacket() {}

bool MachoPacket::setattr( std::string attr_name, PyObject * attr_obj )
{
    /* missing some stuff? */
    return mDict->set_item(attr_name.c_str(), attr_obj);
}

MachoAddress* MachoPacket::get_src_addr()
{
    MachoAddress* result = (MachoAddress*)mDict->get_item("source");
    PyIncRef(result); // we increase the ref counter because we are sharing this resource with another
    return result;
}

bool MachoPacket::init( PyObject* state )
{
    /* check for obj */
    if (!state)
        return false;

    /* check for obj type */
    if (!PyTuple_Check(state))
        return false;

    PyTuple * pState = (PyTuple *)state;

    if (pState->size() != 6) {
        sLog.Error("machoNetPacket", "payload obj size != 6");
        return false;
    }

    mDict->set_item("command", pState->get_item(0));
    mDict->set_item("source", pState->get_item(1));
    mDict->set_item("destination", pState->get_item(2));
    mDict->set_item("userID", pState->get_item(3));
    mDict->set_item("body", pState->get_item(4)); // this one isn't "self.body"
    mDict->set_item("oob", pState->get_item(5));

    // we possible could get this from 
    PyDict * oob = (PyDict *)mDict->get_item("oob");

    assert(oob);

    if (PyDict_Check(oob)) {
        mDict->set_item("compressedPart", oob->get_item("compressedPart", (PyObject*)new PyInt(0))); // default would be PyInt(0);
    }

    if (params.size() != 0) {

        PyTuple* body = (PyTuple*)mDict->get_item("body");

        assert(params.size() == body->size());

        for (unsigned int i = 0; i < params.size(); i++)
        {
            std::string tmp = params[i];
            size_t offset = tmp.find('?');
            assert(offset == size_t(-1));
            setattr(tmp, body->get_item(i));
        }
    }
    return false;
}

PyTuple* MachoPacket::GetState()
{
    return NULL;
}

bool MachoPacket::repr( FILE* fp )
{
    if (*mName == "macho.AuthenticationReq")
    {
        //into+="Packet::AuthenticationReq(%s,%s,%s,%s,%s)";
        fprintf(fp, "Packet::AuthenticationReq(");
        return true;
    }


    PyObject* source = mDict->get_item("source");
    PyObject* destination = mDict->get_item("destination");
    PyObject* payload = mDict->get_item("body"); // naming here is wrong.
    PyObject* oob = mDict->get_item("oob");

    //fprintf(fp, "Packet::%s (%s,%s,PAYLOAD(%d bytes),%s)",

    fprintf(fp, "Packet::%s (", mName->content());

    Dump(fp, source, -1, true);         fprintf(fp, ", ");
    Dump(fp, destination, -1, true);    fprintf(fp, ", ");
    fprintf(fp, "PAYLOAD( ... bytes), "); // fix this better...
    Dump(fp, oob, size_t(-1), true, true);fprintf(fp, ")\n");

    return true;
}

