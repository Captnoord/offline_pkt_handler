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

class MachoAddress;

class MachoPacket : public PyClass
{
protected:
public:
    MachoPacket(const char* derived_name);
    
    ~MachoPacket();;
    void destruct() {}

    virtual MachoPacket* New() = 0;

    bool setattr(std::string attr_name, PyObject * attr_obj);

    MachoAddress* get_src_addr();

    bool init(PyObject* state);

    /* this is a stub that needs to be implemented */
    PyTuple* GetState();

    bool repr( FILE* fp );


protected:
    uint8 command;                      // hmmm should we use normal types for this
    std::vector<std::string> params;    // should we use normal types for this
};

/* @note does this need the GetStage call? */
class macho_CallReq : public MachoPacket
{
public:
    macho_CallReq() : MachoPacket("macho.CallReq")
    {
        params.push_back("payload");
    }

    ~macho_CallReq() {}

    macho_CallReq* New()
    {
        return new macho_CallReq();
    }
};

/* @note does this need the GetStage call? */
class macho_CallRsp : public MachoPacket
{
public:
    macho_CallRsp() : MachoPacket("macho.CallRsp")
    {
        params.push_back("payload");
    }

    ~macho_CallRsp(){}

    macho_CallRsp* New()
    {
        return new macho_CallRsp();
    }
};

/* @note does this need the GetStage call? */
class macho_SessionChangeNotification : public MachoPacket
{
public:
    macho_SessionChangeNotification() : MachoPacket("macho.SessionChangeNotification")
    {
        params.push_back("change");
        params.push_back("nodesOfInterest");
    }

    ~macho_SessionChangeNotification(){}

    macho_SessionChangeNotification* New()
    {
        return new macho_SessionChangeNotification();
    }
};

/* @note does this need the GetStage call? */
class macho_SessionInitialStateNotification : public MachoPacket
{
public:
    macho_SessionInitialStateNotification() : MachoPacket("macho.SessionInitialStateNotification")
    {
        params.push_back("initialstate");
    }

    ~macho_SessionInitialStateNotification(){}

    macho_SessionInitialStateNotification* New()
    {
        return new macho_SessionInitialStateNotification();
    }
};

/* @note does this need the GetStage call? */
class macho_PingRsp : public MachoPacket
{
public:
    macho_PingRsp() : MachoPacket("macho.PingRsp")
    {
        params.push_back("times");
    }

    ~macho_PingRsp(){}

    macho_PingRsp* New()
    {
        return new macho_PingRsp();
    }
};

/* @note does this need the GetStage call? */
class macho_PingReq : public MachoPacket
{
public:
    macho_PingReq() : MachoPacket("macho.PingReq")
    {
        params.push_back("times");
    }

    ~macho_PingReq(){}

    macho_PingReq* New()
    {
        return new macho_PingReq();
    }
};

/* @note does this need the GetStage call? */
class macho_ErrorResponse : public MachoPacket
{
public:
    macho_ErrorResponse() : MachoPacket("macho.ErrorResponse")
    {
        params.push_back("originalCommand");
        params.push_back("code");
        params.push_back("payload");
    }

    ~macho_ErrorResponse(){}

    macho_ErrorResponse* New()
    {
        return new macho_ErrorResponse();
    }
};

/* @note does this need the GetStage call? */
class macho_Notification : public MachoPacket
{
public:
    macho_Notification() : MachoPacket("macho.Notification")
    {
        params.push_back("payload");
    }

    ~macho_Notification(){}

    macho_Notification* New()
    {
        return new macho_Notification();
    }
};

#endif // machoNetPacket_h__
