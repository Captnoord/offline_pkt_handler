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
	Author:		Zhur
*/

#ifndef EVE_MARSHAL_OPCODES_H
#define EVE_MARSHAL_OPCODES_H

typedef enum _PyRepOpcodes
{
	Op_PyNone				= 0x01u,	//1
	Op_PyGlobal		        = 0x02u,	//2		usually a type, function or class object, but just the name,
                                        //      so it has to exist for this to decode properly.

	Op_PyLongLong			= 0x03u,	//3
	Op_PyLong				= 0x04u,	//4
	Op_PySignedShort		= 0x05u,	//4
	Op_PyByte				= 0x06u,	//6
	Op_PyMinusOne			= 0x07u,	//7
	Op_PyZero		        = 0x08u,	//8
	Op_PyOne	    		= 0x09u,	//9
	Op_PyFloat				= 0x0Au,	//10
	Op_PyFloat0			    = 0x0Bu,	//11
	//Op_PyComplex			= 0x0Cu,	//12    (not used, complex number)
	Op_PyLongString			= 0x0Du,	//13	| for strings longer than 255
	Op_PyEmptyString		= 0x0Eu,	//14
	Op_PyCharString			= 0x0Fu,	//15
	Op_PyShortString		= 0x10u,	//16
	Op_PyStringTableItem	= 0x11u,	//17
	Op_PyUnicodeString		= 0x12u,	//18	| full unicode string
	Op_PyBuffer	            = 0x13u,	//19
	Op_PyTuple				= 0x14u,	//20
	Op_PyList				= 0x15u,	//21
	Op_PyDict				= 0x16u,	//22
	Op_PyInstance		    = 0x17u,	//23	| class instance, name of the class follows (as string, probably)
	Op_PyBlue			    = 0x18u,	//24	| blue object
	Op_PyCallback		    = 0x19u,	//25	| callback
	Op_PyLoadcPickledObject	= 0x1Au,	//26	| only used to store / load cached objects.
	Op_PyRef                = 0x1Bu,	//27	| reference to a stored object, 
	Op_PyChecksumedStream	= 0x1Cu,	//28	| default client packet container, hash + data.
	//Op_PyUnused2			= 0x1Du,	//29
	//Op_PyUnused3			= 0x1Eu,	//30
	Op_PyTrue				= 0x1Fu,	//31
	Op_PyFalse				= 0x20u,	//32
	Op_PycPicked			= 0x21u,	//33
	Op_PyReduce		        = 0x22u,	//34	| reduce protocol
	Op_PyNewObj		        = 0x23u,	//35	| new style class object
	Op_PyEmptyTuple			= 0x24u,	//36
	Op_PyOneTuple			= 0x25u,	//37
	Op_PyEmptyList			= 0x26u,	//38
	Op_PyOneList			= 0x27u,	//39
	Op_PyUnicodeEmptyString = 0x28u,	//40	| a empty unicode string
	Op_PyUnicodeCharString	= 0x29u,	//41	| a single unicode character string
	Op_PyDBRow				= 0x2Au,	//42	| still working on this one...
	Op_PyStream			    = 0x2Bu,	//43	| a embeded marshal stream.
	Op_PyTwoTuple			= 0x2Cu,	//44	| a static tuple containing 2 items.
	Op_PyStopIter			= '-',		//45	| 
	Op_PyUnicodeUTF8String	= 0x2Eu,	//46	| unicode string send as UTF8 string.
	Op_PyVarInteger			= 0x2Fu,	//47	| variable length integer field, PyLong from Byte array
	PyRepOpcodeMask			= 0x3Fu,	
	PyOpcodePadding			= 0xFFu,	// make sure the compiler doesn't do strange tricks with this.
	PyOpcodeError			= 0xFFu,
} PyRepOpcodes;	//6 bits

static const uint8 PyRepSaveMask = 0x40u;
static const uint8 PyRepUnknownMask = 0x80u;
static const uint8 SubStreamHeaderByte = '~';	//'~'

#endif//EVE_MARSHAL_OPCODES_H
