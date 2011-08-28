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

#ifndef SERVER_FRAMEWORK_COMMON_H
#define SERVER_FRAMEWORK_COMMON_H

//#define PACKET_PARSER_MEMORY_ON_DRUGS
#undef PACKET_PARSER_MEMORY_ON_DRUGS

#ifdef WIN32
//#  pragma warning(disable:4996)
#  pragma warning(disable:4251)							// 'dll'-interface warning, related to objects that doesn't have a 'declspec' definition

// VC9 related 'CRT' warnings
// TODO check if these warnings can be fixed in a cross platform way
#  define _CRT_SECURE_NO_WARNINGS 1
#  define _CRT_SECURE_NO_DEPRECATE 1
#  define _CRT_SECURE_COPP_OVERLOAD_STANDARD_NAMES 1
//#  define _HAS_ITERATOR_DEBUGGING 0						// Speeds up debug build
#endif//WIN32

#ifdef _DEBUG
#  define _CRTDBG_MAP_ALLOC
#  include <stdlib.h>
#  include <crtdbg.h>
#endif


enum TimeVariables
{
	TIME_SECOND = 1,
	TIME_MINUTE = TIME_SECOND * 60,
	TIME_HOUR   = TIME_MINUTE * 60,
	TIME_DAY	= TIME_HOUR * 24,
	TIME_MONTH	= TIME_DAY * 30,
	TIME_YEAR	= TIME_MONTH * 12
};

enum MsTimeVariables
{
	MSTIME_SECOND = 1000,
	MSTIME_MINUTE = MSTIME_SECOND * 60,
	MSTIME_HOUR   = MSTIME_MINUTE * 60,
	MSTIME_DAY	= MSTIME_HOUR * 24
};

/* 'inlined' functions can improve performance, the compiler will judge how this will be handled.
 * '__forceinline' functions can improve performance but only under certain circumstances
 * url: http://msdn.microsoft.com/en-us/library/z8y1yy88(VS.80).aspx
 */
#ifdef WIN32
#  ifdef _DEBUG
#    define ASCENT_INLINE
#    define ASCENT_FORCEINLINE __forceinline
#  else
#    define ASCENT_INLINE inline
#    define ASCENT_FORCEINLINE __forceinline
#  endif//_DEBUG
#else
#  define ASCENT_INLINE inline
#  define ASCENT_FORCEINLINE inline
#endif//WIN32

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "AscentConfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
//#  define WIN32_LEAN_AND_MEAN
//#  define VC_EXTRALEAN
//#  define STRICT
//#  define NOSOUND
//#  define NOGDI
//#define NOCOMM
#  ifndef _WIN32_WINNT
#    define _WIN32_WINNT 0x0500
#  endif
//#  define NOMINMAX
#  include <windows.h>
#else
#  include <string.h>
#  define MAX_PATH 1024
#endif

//#include "../Tools/DbgHeap/DbgHeap.h"

// Enable invalid memory access detection mode
//extern DbgHeap g_Heap; // the global debug heap object.

// fast hack bla bla...
//void* operator new (size_t nSize);
//void operator delete (void* pPtr);


/*#ifdef min
#  undef min
#endif

#ifdef max
#  undef max
#endif
*/

#ifdef CONFIG_USE_SELECT
#  undef  FD_SETSIZE
#  define FD_SETSIZE 2048 
#endif

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
//#  include <winsock2.h>
//#  include <ws2tcpip.h>
#else
#  include <sys/time.h>
#  include <sys/types.h>
#  include <sys/ioctl.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <unistd.h>
#  include <signal.h>
#  include <netdb.h>
#endif

// current platform and compiler
#define PLATFORM_WIN32 0
#define PLATFORM_UNIX  1
#define PLATFORM_APPLE 2

#define UNIX_FLAVOUR_LINUX 1
#define UNIX_FLAVOUR_BSD 2
#define UNIX_FLAVOUR_OTHER 3
#define UNIX_FLAVOUR_OSX 4

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#  define ASCENT_PLATFORM PLATFORM_WIN32
#elif defined( __APPLE_CC__ )
#  define ASCENT_PLATFORM PLATFORM_APPLE
#else
#  define ASCENT_PLATFORM PLATFORM_UNIX
#endif

#define COMPILER_MICROSOFT 0
#define COMPILER_GNU	   1
#define COMPILER_BORLAND   2

#ifdef _MSC_VER
#  define ASCENT_COMPILER COMPILER_MICROSOFT
#elif defined( __BORLANDC__ )
#  define ASCENT_COMPILER COMPILER_BORLAND
#elif defined( __GNUC__ )
#  define ASCENT_COMPILER COMPILER_GNU
#else
#  pragma error "FATAL ERROR: Unknown compiler."
#endif

#if ASCENT_PLATFORM == PLATFORM_UNIX || ASCENT_PLATFORM == PLATFORM_APPLE
#  ifdef HAVE_DARWIN
#    define PLATFORM_TEXT "MacOSX"
#    define UNIX_FLAVOUR UNIX_FLAVOUR_OSX
#  else
#    ifdef USE_KQUEUE
#      define PLATFORM_TEXT "FreeBSD"
#      define UNIX_FLAVOUR UNIX_FLAVOUR_BSD
#    else
#      ifdef USE_KQUEUE_DFLY
#        define PLATFORM_TEXT "DragonFlyBSD"
#        define UNIX_FLAVOUR UNIX_FLAVOUR_BSD
#      else
#        define PLATFORM_TEXT "Linux"
#        define UNIX_FLAVOUR UNIX_FLAVOUR_LINUX
#      endif
#    endif
#  endif
#endif

#if ASCENT_PLATFORM == PLATFORM_WIN32
#  define PLATFORM_TEXT "Win32"
#endif

#ifdef _DEBUG
#  define CONFIG "Debug"
#else
#  define CONFIG "Release"
#endif

#ifdef X64
#  define ARCH "X64"
#else
#  define ARCH "X86"
#endif

#if ASCENT_PLATFORM == PLATFORM_WIN32
#  ifdef X64
#    define PLATFORM_AND_ARCH_TEXT "Win64"
#  else
#    define PLATFORM_AND_ARCH_TEXT "Win32"
#  endif
#else
#  define PLATFORM_AND_ARCH_TEXT PLATFORM_TEXT"/"ARCH
#endif

/*#if ASCENT_COMPILER == COMPILER_MICROSOFT
#  pragma warning( disable : 4267 ) // conversion from 'size_t' to 'int', possible loss of data
#  pragma warning( disable : 4311 ) // 'type cast': pointer truncation from HMODULE to uint32
#  pragma warning( disable : 4786 ) // identifier was truncated to '255' characters in the debug information
#  pragma warning( disable : 4146 )
#  pragma warning( disable : 4800 )
#endif*/

/* TODO check out why this here */
#if ASCENT_PLATFORM == PLATFORM_WIN32
#  define STRCASECMP stricmp
#else
#  define STRCASECMP strcasecmp
#endif

#if ASCENT_PLATFORM == PLATFORM_WIN32
#  define ASYNC_NET
#endif

#ifdef USE_EPOLL
#  define CONFIG_USE_EPOLL
#endif

#ifdef USE_KQUEUE
#  define CONFIG_USE_KQUEUE
#endif

#ifdef USE_KQUEUE_DFLY
#  define CONFIG_USE_KQUEUE_DFLY
#endif

#ifdef USE_SELECT
#  define CONFIG_USE_SELECT
#endif

#ifdef USE_POLL
#  define CONFIG_USE_POLL
#endif

/*
#ifdef min
#  undef min
#endif

#ifdef max
#  undef max
#endif
*/

#include <set>
#include <list>
#include <string>
#include <map>
#include <queue>
#include <sstream>
#include <algorithm>

#ifdef WIN32
#include <unordered_map>
#include <unordered_set>
#else
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#endif

#if defined (__GNUC__)
#  define GCC_VERSION (__GNUC__ * 10000 \
					   + __GNUC_MINOR__ * 100 \
					   + __GNUC_PATCHLEVEL__)
#endif

#ifndef WIN32
#  ifndef X64
#    if defined (__GNUC__)
#	   if GCC_VERSION >= 30400
#        ifdef HAVE_DARWIN
#	       define __fastcall
#        else
#    	   define __fastcall __attribute__((__fastcall__))
#        endif
#	   else
#	     define __fastcall __attribute__((__regparm__(3)))
#	   endif
#    else
#	   define __fastcall __attribute__((__fastcall__))
#    endif
#  else
#    define __fastcall  
#  endif
#endif

#if ASCENT_COMPILER == COMPILER_GNU && __GNUC__ >= 3
//#  include <ext/hash_map>
//#  include <ext/hash_set>
//#else
#  include <hash_map>
#  include <hash_set>
#endif

#ifdef _STLPORT_VERSION
#  define HM_NAMESPACE std
   using std::hash_map;
   using std::hash_set;
#elif ASCENT_COMPILER == COMPILER_MICROSOFT && _MSC_VER >= 1300
#  define HM_NAMESPACE std::tr1
//   using HM_NAMESPACE::unordered_map;
//   using HM_NAMESPACE::unordered_set;
#  define ENABLE_SHITTY_STL_HACKS 1

   // hacky stuff for vc++
#  define snprintf _snprintf
#  ifdef vsnprintf
#    undef vsnprintf
#    define vsnprintf _vsnprintf
#  endif

#  define strtok_r strtok_s
#elif ASCENT_COMPILER == COMPILER_INTEL
#  define HM_NAMESPACE std
   using std::hash_map;
   using std::hash_set;
#elif ASCENT_COMPILER == COMPILER_GNU && __GNUC__ >= 3
#  define HM_NAMESPACE __gnu_cxx
   using __gnu_cxx::hash_map;
   using __gnu_cxx::hash_set;

namespace __gnu_cxx
{
	template<> struct hash<unsigned long long>
	{
		size_t operator()(const unsigned long long &__x) const { return (size_t)__x; }
	};
	template<typename T> struct hash<T *>
	{
		size_t operator()(T * const &__x) const { return (size_t)__x; }
	};
};

#else
#  define HM_NAMESPACE std
   using std::hash_map;
#endif

/* Use correct types for x64 platforms, too */
#if ASCENT_COMPILER != COMPILER_GNU
  typedef signed __int64 int64;
  typedef signed __int32 int32;
  typedef signed __int16 int16;
  typedef signed __int8 int8;
  typedef unsigned __int64 uint64;
  typedef unsigned __int32 uint32;
  typedef unsigned __int16 uint16;
  typedef unsigned __int8 uint8;
#else
  typedef int64_t int64;
  typedef int32_t int32;
  typedef int16_t int16;
  typedef int8_t int8;
  typedef uint64_t uint64;
  typedef uint32_t uint32;
  typedef uint16_t uint16;
  typedef uint8_t uint8;
  typedef uint32_t DWORD;
#endif

/* 
Scripting system exports/imports
*/

#ifdef WIN32
#  ifndef SCRIPTLIB
#    define SERVER_DECL __declspec(dllexport)
#    define SCRIPT_DECL __declspec(dllimport)
#  else
#    define SERVER_DECL __declspec(dllimport)
#    define SCRIPT_DECL __declspec(dllexport)
#  endif
#else
#  define SERVER_DECL 
#  define SCRIPT_DECL 
#endif

// Include all threading files
#include <assert.h>

// utilities
#include "Util.h"

// specific macro utilities
#include "MacroUtil.h"

#if ASCENT_COMPILER == COMPILER_MICROSOFT
#  define I64FMT "%016I64X"
#  define I64FMTD "%I64u"
#  define SI64FMTD "%I64d"
#  define snprintf _snprintf
#  define atoll __atoi64

// ISO C++ related macros
#  if _MSC_VER >= 1400
//#    define strdup _strdup
#    define strnicmp _strnicmp
#  endif
#else
#  define stricmp strcasecmp
#  define strnicmp strncasecmp
#  define I64FMT "%016llX"
#  define I64FMTD "%llu"
#  define SI64FMTD "%lld"
#endif

#ifdef atol
#  undef atol
#  define atol(a) strtoul( a, NULL, 10)
#endif

#if ASCENT_COMPILER == COMPILER_MICROSOFT
#  if _MSC_VER >= 1400
#    pragma float_control(push)
#    pragma float_control(precise, on)
#  elif (_MSC_VER < 1300)   // MSVC 6 should be < 1300 | TODO if possible double check if only vc6 has this
#    define for if(true)for // fix buggy MSVC's for variable scoping to be reliable =S
#  endif//_MSC_VER
#endif//ASCENT_COMPILER

// fast int abs
static ASCENT_FORCEINLINE int int32abs( const int value )
{
	return (value ^ (value >> 31)) - (value >> 31);
}

// fast int abs and recast to unsigned
static ASCENT_FORCEINLINE uint32 int32abs2uint32( const int value )
{
	return (uint32)(value ^ (value >> 31)) - (value >> 31);
}

/// Fastest Method of float2int32
static ASCENT_FORCEINLINE int float2int32(const float value)
{
#if !defined(X64) && ASCENT_COMPILER == COMPILER_MICROSOFT 
	int i = 0;
	__asm {
		fld value
		frndint
		fistp i
	}
	return i;
#else
	union { int asInt[2]; double asDouble; } n;
	n.asDouble = value + 6755399441055744.0;

	return n.asInt [0];
#endif
}

/// Fastest Method of double2int32
static ASCENT_FORCEINLINE int double2int32(const double value)
{
#if !defined(X64) && ASCENT_COMPILER == COMPILER_MICROSOFT
	int i = 0;
	__asm {
		fld value
		frndint
		fistp i
	}
	return i;
#else
  union { int asInt[2]; double asDouble; } n;
  n.asDouble = value + 6755399441055744.0;
  return n.asInt [0];
#endif
}

#if ASCENT_COMPILER == COMPILER_MICROSOFT && _MSC_VER >= 1400
#  pragma float_control(pop)
#endif

#ifndef WIN32
#  include <sys/timeb.h>
#endif

ASCENT_FORCEINLINE uint32 now()
{	
#ifdef WIN32
	return GetTickCount();
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#endif
}

#ifndef WIN32
#  define FALSE 0
#  define TRUE 1
#endif

#ifndef WIN32
#  define Sleep(ms) usleep(1000*ms)
#endif

/*#ifdef WIN32
#  ifndef __SHOW_STUPID_WARNINGS__
#    pragma warning(disable:4018)
#    pragma warning(disable:4244)
#    pragma warning(disable:4305) 
#    pragma warning(disable:4748)
#    pragma warning(disable:4800) 
#    pragma warning(disable:4996)
#    pragma warning(disable:4251)
#  endif	  
#endif

#undef INTEL_COMPILER
#ifdef INTEL_COMPILER
#  pragma warning(disable:279)
#  pragma warning(disable:1744)
#  pragma warning(disable:1740)
#endif*/

ASCENT_FORCEINLINE void ASCENT_TOLOWER(std::string& str)
{
	const size_t count = str.length();
	for(size_t i = 0; i < count; ++i)
		str[i] = (char)tolower(str[i]);
};

ASCENT_FORCEINLINE void ASCENT_TOUPPER(std::string& str)
{
	for(size_t i = 0; i < str.length(); ++i)
		str[i] = (char)toupper(str[i]);
};

/* stuff for pragma message, so we can show file and line number */
#ifdef WIN32
#  define __STR2__(x) #x
#  define __STR1__(x) __STR2__(x)
#  define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Warning Msg: "
#  define __LOC2__ __FILE__ "("__STR1__(__LINE__)") : "
#endif//WIN32

#include "Errors.h"

#endif
