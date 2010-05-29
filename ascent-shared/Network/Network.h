/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * Network.h - Include this file in your .cpp files to gain access
 *			 to features offered by this library.
 *
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include "../Log.h"
#include "../NGLog.h"
#include "../Util.h"
#include "CircularBuffer.h"
#include "SocketDefines.h"
#include "SocketOps.h"
#include "Socket.h"

#ifdef CONFIG_USE_IOCP
#  include "SocketMgrWin32.h"
#  include "ListenSocketWin32.h"
#endif//CONFIG_USE_IOCP

#ifdef CONFIG_USE_EPOLL
#  include "SocketMgrLinux.h"
#  include "ListenSocketLinux.h"
#endif//CONFIG_USE_EPOLL

#ifdef CONFIG_USE_KQUEUE
#  include "SocketMgrFreeBSD.h"
#  include "ListenSocketFreeBSD.h"
#endif//CONFIG_USE_KQUEUE

#endif//NETWORK_H_
