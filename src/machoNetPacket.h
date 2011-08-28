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

#ifndef machoNetPacket_h__
#define machoNetPacket_h__

#include "Python.h"

#ifdef ENABLE_PACKED_CLASSES
#  pragma pack(push,1)
#endif

class MachoAddress;

class MachoPacket : public PyClass
{
protected:
public:
    MachoPacket(const char* derived_name);
    
    virtual ~MachoPacket();

    virtual MachoPacket* New() = 0;

    bool setattr(std::string attr_name, PyObject * attr_obj);

    MachoAddress* get_src_addr();

    bool init(PyObject* state);

    bool setstate(PyObject* state)
    {
        sLog.Warning("util.row", "stub setstate called");
        return true;
    }

    /* this is a stub that needs to be implemented */
    PyTuple* getstate();

    bool repr( FILE* fp );


protected:
    uint8 command;                      // hmmm should we use normal types for this
    std::vector<std::string> params;    // should we use normal types for this
};

/* @note does this need the GetStage call? */
class machoCallReq : public MachoPacket
{
public:
    machoCallReq();

    ~machoCallReq();

    machoCallReq* New();
};

/* @note does this need the GetStage call? */
class machoCallRsp : public MachoPacket
{
public:
    machoCallRsp();

    ~machoCallRsp();

    machoCallRsp* New();
};

/* @note does this need the GetStage call? */
class machoSessionChangeNotification : public MachoPacket
{
public:
    machoSessionChangeNotification();

    ~machoSessionChangeNotification();

    machoSessionChangeNotification* New();
};

/* @note does this need the GetStage call? */
class machoSessionInitialStateNotification : public MachoPacket
{
public:
    machoSessionInitialStateNotification();

    ~machoSessionInitialStateNotification();

    machoSessionInitialStateNotification* New();
};

/* @note does this need the GetStage call? */
class machoPingRsp : public MachoPacket
{
public:
    machoPingRsp();

    ~machoPingRsp();

    machoPingRsp* New();
};

/* @note does this need the GetStage call? */
class machoPingReq : public MachoPacket
{
public:
    machoPingReq();

    ~machoPingReq();

    machoPingReq* New();
};

/* @note does this need the GetStage call? */
class machoErrorResponse : public MachoPacket
{
public:
    machoErrorResponse();

    ~machoErrorResponse();

    machoErrorResponse* New();
};

/* @note does this need the GetStage call? */
class machoNotification : public MachoPacket
{
public:
    machoNotification();

    ~machoNotification();

    machoNotification* New();
};

#ifdef ENABLE_PACKED_CLASSES
#  pragma pack(pop)
#endif

#endif // machoNetPacket_h__
