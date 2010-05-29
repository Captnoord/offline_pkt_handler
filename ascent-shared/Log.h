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
 */

#ifndef ASCENT_FRAMEWORKLOG_H
#define ASCENT_FRAMEWORKLOG_H

#include "Common.h"
#include "Singleton.h"

// console output colors
#ifdef WIN32

#define TRED	FOREGROUND_RED | FOREGROUND_INTENSITY
#define TGREEN	FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define TYELLOW FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
#define TNORMAL FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE
#define TWHITE	TNORMAL | FOREGROUND_INTENSITY
#define TBLUE	FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY

#else

#define TRED	1
#define TGREEN	2
#define TYELLOW 3
#define TNORMAL 4
#define TWHITE	5
#define TBLUE	6

#endif

std::string FormatOutputString(const char * Prefix, const char * Description, bool useTimeStamp);

class SERVER_DECL oLog : public Singleton<oLog> {
public:
  oLog();
  void String( const char * str, ... );
  void Error( const char * err, ... );
  void Basic( const char * str, ... );
  void Detail( const char * str, ... );
  void Debug( const char * str, ... );
  void Menu( const char * str, ... );
  void Warning( const char * str, ... );
  

  void fLogText(const char *text);
  void SetLogging(bool enabled);
  
  void Init(int32 fileLogLevel, int32 screenLogLevel);
  void SetFileLoggingLevel(int32 level);
  void SetScreenLoggingLevel(int32 level);

  void Color(uint32 colorcode, const char * str, ...);
  
#ifdef WIN32
  HANDLE stdout_handle, stderr_handle;
#endif
  int32 m_fileLogLevel;
  int32 m_screenLogLevel;
private:
	void SetColor(unsigned int color);
};

class SessionLogWriter
{
	FILE * m_file;
	char * m_filename;
public:
	SessionLogWriter(const char * filename, bool open);
	~SessionLogWriter();

	void write(const char* format, ...);
	//void writefromsession(WorldSession* session, const char* format, ...);
	ASCENT_INLINE bool IsOpen() { return (m_file != NULL); }
	void Open();
	void Close();
};

// universal logger
#define sLog oLog::getSingleton()

#endif

