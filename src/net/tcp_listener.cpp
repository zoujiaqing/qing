/**
*  COPYRIGHT NOTICE
*  Copyright (c) 2015, Brian Zou All rights reserved.
*
*  @file tcp_listener.cpp
*  @brief ...
*
*  Asynchronous network library
*
*  @author Brian Zou (zoujiaqing@gmail.com)
*  @date   2015/12/26
*/

#include <qing/net/define.hpp>
#include <qing/net/address.hpp>
#include <qing/net/poller.hpp>
#include <qing/net/tcp_socket.hpp>
#include <qing/net/tcp_listener.hpp>
#include <qing/net/util.hpp>

namespace qing
{
    namespace net
    {
		TcpListener::TcpListener(const std::string& ip, uint16_t port) : _address(ip, port), _pollerPtr(PollerPtr(new Poller()))
		{

		}

		TcpListener::TcpListener(uint16_t port) : _address(port), _pollerPtr(PollerPtr(new Poller()))
		{

		}

		TcpListener::TcpListener(const Address& address) : _address(address), _pollerPtr(PollerPtr(new Poller()))
		{

		}

		TcpListener::~TcpListener()
        {
            this->_acceptHandler = nullptr;
            this->_pollerPtr = nullptr;
        };

		void TcpListener::registerAcceptHandler(const AcceptHandler& handler)
		{
			this->_acceptHandler = handler;
		}

#ifdef HAVE_IOCP
		bool TcpListener::listen()
		{
			/* 创建一个 Listener socket */
			this->_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			struct sockaddr_in sockAddr = this->_address.sockAddr();

			uint32_t optionValue = 1;
			if (-1 == setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&optionValue, sizeof(int)))
			{
				QLOG_ERROR("setsocketopt error");
				return false;
			}

			if (-1 == ::bind(this->_socket, (struct sockaddr*)&sockAddr, sizeof(struct sockaddr)))
			{
				QLOG_ERROR("socket bind faild, errno %d.", errno);
				return false;
			}

			if (-1 == ::listen(this->_socket, 8))
			{
				QLOG_ERROR("socket listen faild, errno %d.", errno);
				return false;
			}

			this->_pollerPtr->add(this, IO_EVENT_TYPE::EVENT_READ);

			/* IOCP模式下注册回调的时候预先产生一个等待连接的SOCKET */
			return this->accept() ? true : false;
		}

#elif HAVE_EPOLL

		bool TcpListener::listen()
		{
			this->_socket = Util::getNewSocket();
			struct sockaddr_in sockAddr = this->_address.sockAddr();

			int i = 1;
			if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, (void*)&i, sizeof(int)) == -1)
			{
				QLOG_ERROR("setsockopt error");
				return false;
			}

			if (::bind(this->_socket, (struct sockaddr*)&sockAddr, sizeof(struct sockaddr)) == -1)
			{
				QLOG_ERROR("socket bind faild, errno %d.", errno);
				return false;
			}

			if (::listen(this->_socket, 8) == -1)
			{
				QLOG_ERROR("socket listen faild, errno %d.", errno);
				return false;
			}

			SocketMgr()->RegistAcceptor(this->_socket, this);

			this->_listening = true;
			runAcceptProcess();
			return true;
		}

#elif HAVE_KQUEUE

		bool TcpListener::listen()
		{
			this->_socket = Util::getNewSocket();
			struct sockaddr_in sockAddr = this->_address.sockAddr();

			int i = 1;
			if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, (void*)&i, sizeof(int)) == -1)
			{
				QLOG_ERROR("setsockopt error");
				return false;
			}

			if (::bind(this->_socket, (struct sockaddr*)&sockAddr, sizeof(struct sockaddr)) == -1)
			{
				QLOG_ERROR("socket bind faild, errno %d.", errno);
				return false;
			}

			if (::listen(this->_socket, 8) == -1)
			{
				QLOG_ERROR("socket listen faild, errno %d.", errno);
				return false;
			}

			SocketMgr()->RegistAcceptor(this->_socket, this);

			this->_listening = true;

		}
#endif

		socket_t TcpListener::accept()
		{
			socket_t socket = Util::getNewSocket();

			if (socket == -1)
			{
				QLOG_ERROR("socket is net valid!");
				return false;
			}

			/* 设置异步 IO 数据内容 */
			memset(&socket, 0, sizeof(IO_DATA));
			memset(_ioData.data, 0, sizeof(_ioData.data));

			_ioData.type = IO_TYPE::HANDLE_ACCEPT;
			_ioData.socket = _socket;

			if (!AcceptEx(_socket,
				socket,
				_ioData.data,
				0,
				sizeof(struct sockaddr_in) + 16,
				sizeof(struct sockaddr_in) + 16,
				&_ioData.bytes,
				&_ioData))
			{
				uint32_t wsa_last_error = WSAGetLastError();
				if (wsa_last_error != ERROR_SUCCESS && wsa_last_error != ERROR_IO_PENDING)
				{
					QLOG_ERROR("AcceptEx error %d, ip: %s, port: %d", wsa_last_error, inet_ntoa(this->_address.sockAddr().sin_addr), ntohs(this->_address.sockAddr().sin_port));
					return -1;
				}
			}

			return socket;
		}

		socket_t TcpListener::accept()
		{

			struct sockaddr_in client_address;
			socklen_t length = sizeof(struct sockaddr_in);

			socket_t socket = ::accept(this->socket(), (sockaddr *)&client_address, &length);
			if (socket < 0 || socket == 0)
			{
				return socket;
			}

			TcpSocket* socketPtr = new TcpSocket(this->_pollerPtr, socket);

			this->_pollerPtr->add(socketPtr, IO_EVENT_TYPE::EVENT_READ);

			return socket;
		}

		void TcpListener::onRead()
		{
			socket_t socket = this->accept();
			this->onAccept(socket);
		}

		void TcpListener::onAccept(socket_t socket)
		{
			// TODO
		}

		void TcpListener::registerAcceptHandler(const AcceptHandler& handler)
		{
			this->_acceptHandler = handler;
		}
    }
}
