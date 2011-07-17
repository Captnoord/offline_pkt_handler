/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2008 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:		Captnoord
 *
 */

#ifndef _ASCENT_H
#define _ASCENT_H

#include "AscentConfig.h"
#include "Common.h"


#include "DebugTools.h"

#include "Singleton.h"

#include "Log.h"
#include "NGLog.h"

#include "Timer.h"

// util fast queue
#include "FastQueue.h"
#include "ByteBuffer.h"
#include "string_map.h"

// threading
#include "Threading/Threading.h"
#include "Threading/RWLock.h"

// network
//#include "Network/Network.h"

// database
//#include "Database/Field.h"
//#include "Database/Database.h"

/*#if defined(ENABLE_DATABASE_MYSQL)
#include "Database/MySQLDatabase.h"
#endif

#if defined(ENABLE_DATABASE_POSTGRES)
#include "Database/PostgresDatabase.h"
#endif

#if defined(ENABLE_DATABASE_SQLITE)
#include "Database/SQLiteDatabase.h"
#endif*/

#endif // _ASCENT_H
