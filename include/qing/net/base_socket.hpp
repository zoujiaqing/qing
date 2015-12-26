/**
*  COPYRIGHT NOTICE
*  Copyright (c) 2015, Brian Zou All rights reserved.
*
*  @file base_socket.hpp
*  @brief ...
*
*  Asynchronous network library
*
*  @author Brian Zou (zoujiaqing@gmail.com)
*  @date   2015/12/26
*/

#pragma once

#include <qing/net/define.hpp>

namespace qing
{
	namespace net
	{
		class Poller;
		
		class BaseSocket
		{
		public:
			BaseSocket();
			BaseSocket(PollerPtr poller);
			BaseSocket(PollerPtr poller, socket_t socket);

			virtual ~BaseSocket();

			virtual void            init() = 0;
			virtual void            close() = 0;

			virtual bool            write(const char* data, uint32_t length) = 0;
			virtual bool            read() = 0;

			virtual void            onWrite() = 0;
			virtual void            onRead() = 0;
			virtual void            onError() = 0;

			virtual bool			valid();
			virtual socket_t		socket();
			virtual void			setSocket(socket_t handle);
			virtual void			setPoller(PollerPtr poller);
			virtual PollerPtr		getPoller();

		protected:
			socket_t	_socket = INVALID_SOCKET;
			PollerPtr	_pollerPtr = nullptr;
		};
	}
}
