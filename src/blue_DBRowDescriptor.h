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

class blue_DBRowDescriptor : public PyClass
{
protected:
public:
    blue_DBRowDescriptor() : PyClass( "blue.DBRowDescriptor" )
    {
        is_c_api = true; // bleh hack...
    }

    ~blue_DBRowDescriptor()
    {
    };

    void destruct() {}

    blue_DBRowDescriptor* New()
    {
        return new blue_DBRowDescriptor();
    }

    // comments: format guessed from compiled scripts
    bool init(PyObject* state)
    {
        // TODO split Class and CClass
        if (state->gettype() == PyTypeClass)
        {
            // not implemented yet
            ASCENT_HARDWARE_BREAKPOINT;
        }
        else
        {
            if (!PyTuple_Check(state))
            {
                //ASCENT_HARDWARE_BREAKPOINT; // this is kinda correct... it needs a hell lot more... can't explain... need to reverse..
                //DumpObject(stdout, state);
                PyTuple* pTuple = (PyTuple*)state;
                PyTuple* pSubTuple = (PyTuple*)pTuple->get_item(0);
                for (unsigned int i = 0; i < pSubTuple->size(); i++)
                {
                    PyTuple* itemTuple = (PyTuple*)pSubTuple->get_item(i);
                    assert(itemTuple);
                    
                    assert(itemTuple->size() == 2);
                    // bleh this is evil... aka fucked up..
                    mTableNames.push_back(itemTuple->GetItem_asPyString(0)->content());
                    mTableTypes.push_back(itemTuple->GetItem_asInt(1));
                    mTableSizes.push_back(1); // dirty hack I am surely gonna forget... we haven't implemented the shit for gettypesize..
                }
            }
            else
            {
                sLog.Error("DBRowDescriptor", "expected tuple of tuples");
                ASCENT_HARDWARE_BREAKPOINT;
                return false;
            }
        }

        return true;
    }

    PyTuple* GetState()
    {
        return NULL;
    };

    // this is really is CPythonClass
    bool is_c_api;

    // clean this up so it only contains the stuff we need... more compact
    std::vector <std::string>   mTableNames;
    std::vector <int32>         mTableTypes;
    std::vector <uint32>        mTableSizes;
};
