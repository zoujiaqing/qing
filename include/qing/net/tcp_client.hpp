/**
*  COPYRIGHT NOTICE
*  Copyright (c) 2015, Brian Zou All rights reserved.
*
*  @file tcp_client.hpp
*  @brief ...
*
*  Asynchronous network library
*
*  @author Brian Zou (zoujiaqing@gmail.com)
*  @date   2015/12/26
*/

#pragma once

#include <qing/util/time_engine.h>

namespace qing
{
	namespace net
	{
		class TcpClient : public TcpSocket
		{
		public:

			TcpClient();

			virtual ~TcpClient();

			void run();
			void stop() { _bIsRun = false; }

			virtual void onConnect(socket_t sock_id, int serverType) = 0;

			bool connect(std::string ip, unsigned short port, int serverType);
			bool connect(SOCKADDR_IN& sin, int serverType);

			void setTimeout(uint32_t timeout_seconds);

            TimerThread& timer_engine(){ return _time_engine; }
		protected:
			int sock_connect(socket_t sock_id, std::string ip, unsigned short port);
			int sock_connect(socket_t sock_id, SOCKADDR_IN& sin);
			bool _connect(std::string ip, unsigned short port, int serverType);

            std::map<socket_t, uint32_t>  _map_connected;
			/* global poller ptr */
			PollerPtr				_pollerPtr = nullptr;

			/* timeout seconds */
			uint32_t _timeout = 1;

			/* ip address */
			//Address				_address;

            TimerThread         _time_engine;
            bool                _bIsRun = false;
		};
	}
}
