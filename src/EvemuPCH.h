/*  EVEmu: EVE Online Server Emulator

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY except by those people which sell it, which
  are required to give you total support for your newly bought product;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
	
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __EVEPCH_H
#define __EVEPCH_H

/* this enables the remarshaling of the unmarshaled object to check if our unmarshal and marshal code is in sync */
//#define MARSHAL_REMARSHAL

//#include <vld.h>

#include <algorithm>

#include "ascent.h"

#include "TimeMeasure.h"

#include "BufferPool.h"
#include "PyStringTable.h"

/* python fake objects */
#include "PyObjects.h"
#include "PyUnicodeUCS2.h"
#include "PyString.h"

#include "PyChameleon.h"
#include "PyObjectDeleter.h"

#include "RleModule.h"
#include "BinAsciiModule.h"
#include "HexAsciiModule.h"

/* marshal stream includes */
#include "ReadStream.h"
#include "WriteStream.h"
#include "MarshalReferenceMap.h"
#include "MarshalStream.h"

#include "packet_types.h"
#include "PyObjectDumper.h"
#include "DatabaseDumpModule.h"

#endif//__EVEPCH_H
