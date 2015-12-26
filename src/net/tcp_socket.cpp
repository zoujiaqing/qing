/**
*  COPYRIGHT NOTICE
*  Copyright (c) 2015, Brian Zou All rights reserved.
*
*  @file tcp_socket.cpp
*  @brief ...
*
*  Asynchronous network library
*
*  @author Brian Zou (zoujiaqing@gmail.com)
*  @date   2015/12/26
*/

#include <qing/net/define.hpp>
#include <qing/net/address.hpp>
#include <qing/net/tcp_socket.hpp>
#include <qing/net/poller.hpp>
#include <qing/net/util.hpp>

namespace qing
{
	namespace net
	{
        TcpSocket::TcpSocket()
        {
			this->_socket = Util::getNewSocket();

			if (!this->valid())
			{
				QLOG_ERROR("create socket error.");
			}

			this->init();
        }

		TcpSocket::TcpSocket(PollerPtr poller) : _pollerPtr(poller)
		{

		}

		TcpSocket::TcpSocket(PollerPtr poller, socket_t socket) : _pollerPtr(poller)
		{
			setSocket(socket);

			if (!this->valid())
			{
				QLOG_ERROR("create socket error.");
			}

			this->init();
		}

		TcpSocket::~TcpSocket()
		{
			QLOG_DEBUG("~TcpSocket");

			_socket = -1;
			_pollerPtr = nullptr;
			_sendHandler = nullptr;
			_recvHandler = nullptr;
			_closeHandler = nullptr;
		}

		void TcpSocket::init()
		{
			/* 初始化 io_data 内存空间，否则会报错 */
			memset(&this->_ioData, 0, sizeof(IO_DATA));
			this->_ioData.socket = this->_socket;
			this->_ioData.ptr = this;
		}

		void TcpSocket::setAddress(const Address& address)
		{
			this->_address = address;
		}

		Address TcpSocket::address()
		{
			return this->_address;
		}

		void TcpSocket::registerSendHandler(const SendHandler& handler)
		{
			this->_sendHandler = handler;
		}

		void TcpSocket::registerRacvtHandler(const RecvHandler& handler)
		{
			this->_recvHandler = handler;
		}

		void TcpSocket::registerCloseHandler(const CallbackHandler& closeHandler)
		{
			this->_closeHandler = closeHandler;
		}

		void TcpSocket::writeFinished(IO_CODE errorCode, uint32_t length, char* message)
		{
			if (this->_sendHandler != nullptr)
			{
				this->_sendHandler(errorCode, length, message);
			}
		}

		void TcpSocket::readFinished(IO_CODE errorCode)
		{
			if (this->_recvHandler != nullptr)
			{
				this->_recvHandler(errorCode);
			}
		}

		/* socket 是否有效 */
		bool TcpSocket::valid()
		{
			return (this->_socket != -1);
		}

		bool TcpSocket::write(const char* data, uint32_t length)
		{
			if (false == this->valid())
			{
				return false;
			}

			if (length == 0)
			{
				QLOG_ERROR("data length is 0.");
				return false;
			}

			this->_writeBuffer.len = length;
			this->_writeBuffer.buf = data;

			this->_WriteIoData.type = HANDLE_WRITE;
			this->_WriteIoData.bytes = 0;

			uint32_t sendResult = 0;

			sendResult = WSASend(this->_socket, &this->_writeBuffer, 1, &_WriteIoData.bytes, 0, &_WriteIoData, NULL);
			if (sendResult != 0)
			{
				uint32_t err = 0;
				uint32_t wsaLastError = WSAGetLastError();
				if ((sendResult == SOCKET_ERROR) && (WSA_IO_PENDING != (err = wsaLastError)))
				{
					QLOG_ERROR("WSASend failed with error: %d length=%d\n", err, length);
					this->close();
					return;
				}
			}

			return;
		}

		bool TcpSocket::read()
		{
			if (false == this->valid() && _ioData.type == IO_TYPE::HANDLE_COMPLETE)
			{
				return false;
			}

			_readIoData.bytes = 0;
			_readIoData.type = HANDLE_READ;

			DWORD flags = 0;

			if (WSARecv(this->_socket, &this->_readBuffer, 1, &_readIoData.bytes, &flags, &_readIoData, NULL) != 0)
			{
				uint32_t wsa_last_error = WSAGetLastError();
				if (wsa_last_error != ERROR_SUCCESS && wsa_last_error != ERROR_IO_PENDING)
				{
					QLOG_ERROR("WSARecv error %d.", wsa_last_error);
					this->close();
					return false;
				}
			}

			return true;
		}

		void TcpSocket::onWrite()
		{
			if (false)
			{
				//this->sendFinished(CODE_SUCCESS, (int)_writeIoData.bytes, nullptr);
			}
		}

		void TcpSocket::onRead()
		{
			//SocketMgr()->timeBegin("TcpSocket::enqueue");
			if (_ioData.bytes <= 0)
			{
				this->close();
			}
			else
			{
				this->_writeQueue.push(_readBuffer.buf, _ioData.bytes);
			}
		}

		void TcpSocket::close()
		{
			if (false == this->valid())
			{
				return;
			}

			/* 删除SOCKET注册的事件 */

			if (-1 != this->_handle)
			{
				shutIOData();
				closesocket(this->_handle);
			}

			this->_socket = -1;

			if (nullptr != this->_closeHandler)
			{
				this->_closeHandler();
			}
		}
	}
}
