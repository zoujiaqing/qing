/**
*  COPYRIGHT NOTICE
*  Copyright (c) 2015, Brian Zou All rights reserved.
*
*  @file tcp_listener.hpp
*  @brief ...
*
*  Asynchronous network library
*
*  @author Brian Zou (zoujiaqing@gmail.com)
*  @date   2015/12/26
*/

#pragma once

#include <qing/net/define.hpp>
#include <qing/net/base_socket.hpp>
#include <qing/net/address.hpp>

namespace qing
{
    namespace net
    {
		class TcpListener : public BaseSocket
        {
		public:
			TcpListener(const std::string& ip, uint16_t port);
			TcpListener(uint16_t port);
			TcpListener(const Address& address);
			virtual ~TcpListener();

			virtual bool listen();
            virtual void onAccept(socket_t socket) = 0;
			virtual void onRead() = 0;

			void registerAcceptHandler(const AcceptHandler& handler);

			socket_t accept();

		protected:
			Address			_address;
			bool			_listening = false;
            PollerPtr		_pollerPtr = nullptr;
            AcceptHandler	_acceptHandler = nullptr;
			IO_DATA			_ioData;
        };
    }
}
