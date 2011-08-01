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

#ifndef _FILE_PARSER_H
#define _FILE_PARSER_H

#include <stdio.h>

#include "machoNetPacket.h"

FILE* fp_in;
FILE* fp_out;

uint8 Getuint8()
{
	uint8 buff;
	fread(&buff,1,1,fp_in);
	return buff;
}

uint16 Getuint16()
{
	uint16 buff;
	fread(&buff,2,1,fp_in);
	return buff;
}

uint32 Getuint32()
{
	uint32 buff;
	fread(&buff,4,1,fp_in);
	return buff;
}

int32 Getint32()
{
	int32 buff;
	fread(&buff,4,1,fp_in);
	return buff;
}

uint64 Getuint64()
{
	uint64 buff;
	fread(&buff,8,1,fp_in);
	return buff;
}

char* GetBuffer(size_t len)
{
	char* buff = (char*)ASCENT_MALLOC(len);
	assert(buff);
	fread(buff, len,1,fp_in);
	return buff;
}

/* this class represents a macho transport connection...
 */
class MachoTransport
{
public:
    MachoTransport()   {}

    bool write( PyObject* message )
    {
        /*
        if hasattr(self, 'userID'):
            message.userID = self.userID
        if ((message.source.addressType == const.ADDRESS_TYPE_ANY) and (not (message.command % 2))):
            message.source.nodeID = self.machoNet.nodeID
            message.source.addressType = const.ADDRESS_TYPE_NODE
            message.Changed()
        elif ((message.source.addressType == const.ADDRESS_TYPE_NODE) and (message.source.nodeID is None)):
            message.source.nodeID = self.machoNet.nodeID
            message.Changed()
        */

        /* if we got our own userID, force the message userID */

        //macho.CallReq
        if(!PyClass_Check(message))
            return false;

        MachoPacket* macho_message = (MachoPacket*)message;

        /*
        int src_addr_type = macho_message->get_src_addr()->get_addr_type();
        if ( src_addr_type == ADDRESS_TYPE_ANY && macho_message->get_command() % 2)
        {
            //message.source.nodeID = self.machoNet.nodeID
            //message.source.addressType = const.ADDRESS_TYPE_NODE
            //message.Changed()
        }
        else if (src_addr_type == ADDRESS_TYPE_NODE && message)
        {

        }
        */





        return true;
    }

    /*bool read(  )*/

private:
    uint64 userID;
    /**

    * holds the ID of this transport
    transportID

    * could be the transport socket or something... I dono....
    transport

    * the name of this transport
    transportName

    @note I wonder how this will end up with design wise
    if (self.transportName == 'tcp:packet:client'):
        self.userID = None

    * dono... does this transport represents a client? if so whats the clientID
    clientID

    * dono.. its a list of clientID's maybe a node that has {clientIDs} connected to it..
    clientIDs

    * unk and not in the mood to check what it is
    dependants

    * list of sessions that belong to this transport
    sessions

    * list of calls.... dono
    calls

    * even unknown.... how this would work
    currentReaders

    * if we are a client then limit the amount of reader threads, if we are not a client then we can handle 20
    desiredReaders
    if (self.transportName == 'tcp:packet:client'):
        self.desiredReaders = 2
    else:
        self.desiredReaders = 20

    * holds time stamp of last ping time
    lastping

    * unknown
    pinging

    * estimated roundtrip time and timeout
    estimatedRTT

    @note http://vidyaprasar.dei.ac.in/wiki/index.php/TCP_Roundtrip_Time_and_Timeout

    * dono
    timeDiff

    * constant when to compress a packet... default 200 bytes ( optional: read from settings )
    compressionThreshold

    * a compressed packet is only valid when we can compress with to 75% of its original size ( optional: read from settings )
    compressionPercentageThreshold

    * something related to ddossing a server node
    largePacketLogSpamThreshold

    * something related to ddossing a server node default 5000000
    dropPacketThreshold
    */
};

MachoTransport transports[100];

bool _GetTransports( PyObject* address, std::vector<MachoTransport*> &transport )
{
    return true;
}

void HandleFile(const char* in_file_path, const char* out_file_path)
{
	std::string out_path;
	if (out_file_path == NULL)
	{
		out_path = in_file_path;
		out_path.resize(out_path.size() - 4);
		out_path+="_handled.txt";
	}
	else
	{
		out_path = out_file_path;
	}

	fp_in = fopen(in_file_path,"rb");
	fp_out = fopen(out_path.c_str(), "w");

	long fsize = 0;
	fseek(fp_in,0,SEEK_END);
	fsize = ftell(fp_in);
	fseek(fp_in,0,SEEK_SET);

	size_t i = 0;
	while (ftell(fp_in) < fsize)
	{
		uint32 FileOffset = ftell(fp_in);
		time_t timestamp = Getuint64();
		uint8 direction = Getuint8();
		int length = Getint32();
		char* packetBuf = GetBuffer(length);

		//fprintf(fp_out, "\n{%s} FileOffset:0x%X, Packet Nr:%u, length:%u, time:%u\n\n", (direction ? "SERVER" : "CLIENT"), FileOffset, i, length, uint32(timestamp & 0xFFFFFFFF));

        fprintf(fp_out, "\n\n");
		
		ReadStream readstream(packetBuf, length);
        MarshalStream stream;

    	PyObject* henk = stream.load(readstream);

        /*
        transports = self._GetTransports(packet.destination)
        for each in transports:
            each.Write(packet)
            each.TagPacketSizes(packet)
        */

		if (henk != NULL)
		{
			DumpObject(fp_out, henk);
			PyDecRef(henk);
		}

		ASCENT_FREE(packetBuf);
		
		if (readstream.tell()!= readstream.size())
			Log.Warning("OfflinePacketParser","sub stream isn't handled completely: %u - %u", readstream.tell(), readstream.size());

		//printf("Parsed packet nr: %u | offset: %u, size: %u\n", i, readstream.tell(), readstream.size());

		i++;
	}

	fclose(fp_out);
	fclose (fp_in);

	printf("\nOffline packet handling done: handled:%u packets\n", i);
}

#endif//_FILE_PARSER_H
