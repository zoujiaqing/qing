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

#include <algorithm>
#include <atomic>
#include <thread>
#include <mutex>
#include <string.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <functional>
#include <chrono>
#include <map>
#include <unordered_map>
#include <queue>
#include <iterator>
#include <vector>

#include <qing/define.hpp>
#include <qing/util/logger.hpp>

#define _OUT
#define _IN

/* IOCP */
#if defined(HAVE_WINDOWS)

#define HAVE_IOCP
#include <WinSock2.h>
#include <Windows.h>
#include <MSWSock.h>

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "Mswsock")

#endif

/* EPOLL */
#if defined(HAVE_LINUX) || defined(HAVE_ANDROID)
#ifndef HAVE_EPOLL
#define HAVE_EPOLL
#endif // !HAVE_EPOLL
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#define gettid() syscall(__NR_gettid)
#endif

/* KQUEUE */
#if defined(HAVE_FREEBSD) \
	|| defined(HAVE_KFREEBSD) \
	|| defined(HAVE_NETBSD) \
	|| defined(HAVE_OPENBSD) \
	|| defined(HAVE_DARWIN)

#define HAVE_KQUEUE

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#endif

/* NOT SUPPORT */
#if !defined(HAVE_IOCP) \
	&& !defined(HAVE_EPOLL) \
	&& !defined(HAVE_KQUEUE)
#error Operating system does not support(qing/net/define.hpp).
#endif

/* define some ... */
#define ACCPET_THREAD 2
#define MAX_MESSAGE_LEGNTH 20480
#define MAX_SOCKET 65535
#define MAX_EVENT_SIZE 256
#define CLIENT_MAX_SEND_BUFFER_SIZE 1024*1024*10
#define CLIENT_MAX_RECV_BUFFER_SIZE 1024*1024*10
#define SERVER_MAX_SEND_BUFFER_SIZE 1024*1024*1024
#define SERVER_MAX_RECV_BUFFER_SIZE 1024*1024*1024
#define MAX_RECEIVE_BUFFER 10240
#define MAX_SEND_SIZE 655360

#define debug_message_box

enum CONNECT_STATUS
{
    CLIENT_UNCONNECTED = 0,
    CLIENT_CONNECTING,
    CLIENT_CONNECTED
};

#ifndef INFINITE
#define INFINITE			0xFFFFFFFF
#endif
//
//#ifndef INVALID_SOCKET
//#define  INVALID_SOCKET 0
//#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR		-1
#endif

#ifdef HAVE_ANDROID
#define EPOLLONESHOT		0x40000000
#endif

/* type */
#ifdef HAVE_IOCP
typedef SOCKET socket_t;
typedef HANDLE handle_t;
// 		typedef WSABUF buffer_t;
typedef int socklen_t;
#else
typedef int socket_t;
typedef int handle_t;
// 		typedef struct {
// 			ulong_t len;
// 			char* buf;
// 		} buffer_t;
#endif

namespace qing
{
	namespace net
	{
		class TCPConnection;
		class TcpSocket;
		class Acceptor;
		class Poller;

		enum IO_CODE
		{
			CODE_SUCCESS,
			CODE_ERROR,
			CODE_REMOTE_CLOSED,
		};

		/* shared_ptr */
		typedef std::shared_ptr<TCPConnection> TCPConnectionPtr;
		typedef std::shared_ptr<TcpSocket> TcpSocketPtr;
		typedef std::shared_ptr<Acceptor> AcceptorPtr;
		typedef std::shared_ptr<Poller> PollerPtr;

		/* function */
		typedef std::function<void(TcpSocket)> AcceptHandler;
		typedef std::function<void(IO_CODE, uint32_t, char*)> SendHandler;
		typedef std::function<void(IO_CODE)> RecvHandler;
		typedef std::function<void()> CallbackHandler;

		typedef struct {
			uint64_t len;
            char buf[MAX_RECEIVE_BUFFER];
		} buffer_t;

		typedef struct {
			uint64_t len;
			char &ptr;
		} ptrbuffer_t;

		typedef uint32_t EVENT_T;

		/* 目前只定义TCP的3个类型 */
		enum IO_TYPE
		{
			HANDLE_ACCEPT,
			HANDLE_READ,
			HANDLE_WRITE,
			HANDLE_COMPLETE,
		};

		enum IO_EVENT_TYPE
		{
			EVENT_NONE = 0,
			EVENT_OPEN = 1 << 0,
			EVENT_READ = 1 << 1,
			EVENT_WRITE = 1 << 2,
			EVENT_CLOSE = 1 << 3,
			EVENT_ERROR = 1 << 4,
			EVENT_TIMEOUT = 1 << 5,
			EVENT_SIGNAL = 1 << 6,
		};

		/* 异步操作带的DATA数据 */
		struct BASE_IO_DATA
		{
			socket_t	socket;
			IO_TYPE		type;
			void*		ptr;
		};

#ifdef HAVE_IOCP

		struct IO_DATA : BASE_IO_DATA, public OVERLAPPED
		{
			DWORD	bytes;
            char	data[MAX_RECEIVE_BUFFER];
        };
        
#elif defined(HAVE_KQUEUE)
        
        struct IO_DATA : BASE_IO_DATA
        {
            // TODO
        };

#elif defined(HAVE_EPOLL)

		struct IO_DATA : BASE_IO_DATA
		{
			epoll_event	ev;
		};

#endif

	}
} // namespace qing
