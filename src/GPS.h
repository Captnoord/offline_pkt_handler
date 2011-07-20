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

class exceptions_GPSTransportClosed : public PyClass
{
protected:
public:
    exceptions_GPSTransportClosed() : PyClass("exceptions.GPSTransportClosed") {}

    ~exceptions_GPSTransportClosed(){};
    void destruct() {}

    exceptions_GPSTransportClosed* New()
    {
        return new exceptions_GPSTransportClosed();
    }

    /* comments: format guessed from compiled scripts
     * @note not finished yet
     */
    bool init(PyObject* state)
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

    /* this is a stub that needs to be implemented */
    PyTuple* GetState()
    {
        return NULL;
    };

    bool repr( FILE* fp )
    {
        fprintf(fp, "dbutil_CRowset needs some dumping love\n");
        return true;
    }

};
