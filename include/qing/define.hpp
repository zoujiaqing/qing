/**
*  COPYRIGHT NOTICE
*  Copyright (c) 2015, Brian Zou All rights reserved.
*
*  @file define.hpp
*  @brief ...
*
*  Asynchronous network library
*
*  @author Brian Zou (zoujiaqing@gmail.com)
*  @date   2015/12/26
*/

#pragma once

#if defined(_WIN32) || defined(_WINDOWS_)
#define HAVE_WINDOWS 1
#include <process.h>	// for _getpid()
#endif

#if defined(__osf__)
// Tru64 lacks stdint.h, but has inttypes.h which defines a superset of
// what stdint.h would define.
#include <inttypes.h>
#elif !defined(_MSC_VER)
#include <stdint.h>
#endif

#ifdef __linux__
#define HAVE_LINUX 1
#error LINUX.
#endif

#ifdef __FreeBSD__
#define HAVE_FREEBSD 1
#endif

#ifdef __FreeBSD_kernel__
#define HAVE_KFREEBSD 1
#endif

#ifdef __NetBSD__
#define HAVE_NETBSD 1
#endif

#ifdef __OpenBSD__
#define HAVE_OPENBSD 1
#endif

#ifdef __APPLE__
#define HAVE_DARWIN 1
#endif

#ifdef ANDROID
#define HAVE_ANDROID 1
#endif

#if !defined(HAVE_WINDOWS) \
	&& !defined(HAVE_LINUX) \
	&& !defined(HAVE_FREEBSD) \
	&& !defined(HAVE_KFREEBSD) \
	&& !defined(HAVE_NETBSD) \
	&& !defined(HAVE_OPENBSD) \
	&& !defined(HAVE_DARWIN) \
	&& !defined(HAVE_ANDROID)
#error Operating system does not support.
#endif

/* 统一定义基本的数据类型 */

namespace qing
{
	class nocopyable
	{
	protected:
		nocopyable() {}
		~nocopyable() {}
	private:
		nocopyable(const nocopyable&);
		nocopyable& operator=(const nocopyable&);
	};
}

#ifndef HAVE_WINDOWS
#define __FUNCTION__ __PRETTY_FUNCTION__
#endif

#define QING_TRACE()	printf("[TRACE] [%s:%d]\n", __FUNCTION__, __LINE__);

#ifdef HAVE_WINDOWS
#ifndef snprintf
#define snprintf		_snprintf_s
#endif // !snprintf
#define strcat_safe		strcat_s
#define strtok_safe		strtok_s
#define get_pid			_getpid
#else
#define strcat_safe		strcat
#define strtok_safe		strtok_r
#define get_pid			getpid
#endif

/* 让其他平台兼容部分 Windows 常量 */
#ifndef HAVE_WINDOWS
typedef char *LPSTR;
typedef const char *LPCSTR;
typedef unsigned long DWORD;
typedef int BOOL;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr *LPSOCKADDR;
#endif