/**
*  COPYRIGHT NOTICE
*  Copyright (c) 2015, Brian Zou All rights reserved.
*
*  @file tcp_socket.hpp
*  @brief ...
*
*  Asynchronous network library
*
*  @author Brian Zou (zoujiaqing@gmail.com)
*  @date   2015/12/26
*/

#pragma once

#include <qing/net/base_socket.hpp>

#include <queue>

namespace qing
{
	namespace net
	{
		class TcpSocket : public BaseSocket
		{
		public:
            TcpSocket();
			TcpSocket(PollerPtr pollerPtr);
			TcpSocket(PollerPtr pollerPtr, socket_t handle);

			virtual ~TcpSocket();

			virtual void init();
			virtual bool write(const char* data, uint32_t length);
			virtual bool read();
			virtual void onWrite();
			virtual void onRead();
			virtual void onError();
			virtual void close();

		public:
			bool valid();
            Address	address();
			void setAddress(const Address& address);

			void registerSendHandler(const SendHandler& handler);
			void registerRacvtHandler(const RecvHandler& handler);
			void registerCloseHandler(const CallbackHandler& closeHandler);

			void writeFinished(IO_CODE errorCode, uint32_t length, char* message);
			void readFinished(IO_CODE errorCode);

		protected:
			Address					_address;
			PollerPtr				_pollerPtr = nullptr;
			SendHandler				_sendHandler = nullptr;
			RecvHandler				_recvHandler = nullptr;
			CallbackHandler			_closeHandler = nullptr;
			uint32_t				_connStatus = CLIENT_UNCONNECTED;
			WSABUF					_readBuffer;
			WSABUF					_writeBuffer;
			IO_DATA					_readIoData;
			IO_DATA					_WriteIoData;
			std::queue<buffer_t>	_readQueue;
			std::queue<buffer_t>	_writeQueue;
		};
	}
}
