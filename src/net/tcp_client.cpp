/**
*  COPYRIGHT NOTICE
*  Copyright (c) 2015, Brian Zou All rights reserved.
*
*  @file tcp_client.cpp
*  @brief ...
*
*  Asynchronous network library
*
*  @author Brian Zou (zoujiaqing@gmail.com)
*  @date   2015/12/26
*/

#include <qing/net/define.hpp>
#include <qing/net/buffer.hpp>
#include <qing/util/time_engine.h>
#include <qing/net/address.hpp>
#include <qing/net/poller.hpp>
#include <qing/net/tcp_socket.hpp>
#include <qing/net/tcp_client.hpp>

namespace qing
{
	namespace net
	{
		TcpClient::TcpClient()
		{
            _bIsRun = true;
			_pollerPtr = GetSockMgr()->GetPoller();
		}

		TcpClient::~TcpClient()
		{
            QLOG_DEBUG("~TcpClient");
            this->_bIsRun = false;
            GetSockMgr()->StopPoll();
		}

		void TcpClient::run()
		{
			/* 已连接轮询 */
			while (this->_bIsRun)
			{
                _time_engine.Update();
                GetSockMgr()->RunPoll();
			}
		}

		bool TcpClient::connect(std::string ip, unsigned short port, int serverType)
		{
			/* 正在连接状态 */
			return _connect(ip, port, serverType);
			
			///* 使用定时器准备检测连接超时 */
			////this->_timer.createTask(this->_timeout * 1000, true, std::bind(&TcpClient::_checkTimeout, this));
            //this->_connected = CLIENT_CONNECTING;
            //timer_engine().Register(true, 10, std::bind(&TcpClient::_checkTimeout, this));
		}

		bool TcpClient::connect(SOCKADDR_IN& sin, int serverType)
		{
			socket_t sock_id = GetSockMgr()->InitNewSocket();
			GetSockMgr()->RegistSocket(sock_id);
			auto socket_ptr = GetSockMgr()->GetSocketInstance(sock_id);
			socket_ptr->set_conn_status(CLIENT_CONNECTING);

			int status = socket_ptr->conn_status();
			int test_times = 10;
			while (status == CLIENT_CONNECTING && test_times > 0)
			{
				status = sock_connect(socket_ptr->socket(), sin);
				if (status == CLIENT_UNCONNECTED)
				{
					status = CLIENT_CONNECTING;
					test_times--;
				}
			}

			if (test_times <= 0)
			{
				QLOG_DEBUG("connected failed :(");
				status = CLIENT_UNCONNECTED;
				return false;
			}

			QLOG_DEBUG("connected :)");

			this->onConnect(socket_ptr->socket(), serverType);
			return true;
		}

		int TcpClient::sock_connect(socket_t sock_id, std::string ip, unsigned short port)
        {
			Address address(ip, port);

            auto socket_ptr = GetSockMgr()->GetSocketInstance(sock_id);
			if (::connect(sock_id, (sockaddr*)&address.sockAddr(), sizeof(sockaddr)) == SOCKET_ERROR)
            {
                QLOG_ERROR("connect failed.");
                // 超时操作已经返回给 onConnect 了，不需要再回调 onConnect
                if (CLIENT_UNCONNECTED != socket_ptr->conn_status())
                {
                    socket_ptr->set_conn_status(CLIENT_UNCONNECTED);
                }
            }
            else
			{
				_pollerPtr->setNonblocking(sock_id);
                socket_ptr->set_conn_status(CLIENT_CONNECTED);
            }

            return socket_ptr->conn_status();
        }

		int TcpClient::sock_connect(socket_t sock_id, SOCKADDR_IN& sin)
		{
			auto socket_ptr = GetSockMgr()->GetSocketInstance(sock_id);
			if (::connect(sock_id, (LPSOCKADDR)&sin, sizeof(sockaddr)) == SOCKET_ERROR)
			{
				QLOG_ERROR("connect failed.");
				// 超时操作已经返回给 onConnect 了，不需要再回调 onConnect
				if (CLIENT_UNCONNECTED != socket_ptr->conn_status())
				{
					socket_ptr->set_conn_status(CLIENT_UNCONNECTED);
				}
			}
			else
			{
				_pollerPtr->setNonblocking(sock_id);
				socket_ptr->set_conn_status(CLIENT_CONNECTED);
			}

			return socket_ptr->conn_status();
		}

		bool TcpClient::_connect(std::string ip, unsigned short port, int serverType)
		{
            socket_t sock_id = GetSockMgr()->InitNewSocket();
            GetSockMgr()->RegistSocket(sock_id);
            auto socket_ptr = GetSockMgr()->GetSocketInstance(sock_id);

#ifdef HAVE_IOCP
            //timer_engine().Register(true, 10, std::bind(&TcpSocketIocp::_checkTimeout, socket_ptr));
#elif defined HAVE_KQUEUE
            //timer_engine().Register(true, 10, std::bind(&TcpSocketKqueue::_checkTimeout, socket_ptr));
#elif defined HAVE_EPOLL
            //timer_engine().Register(true, 10, std::bind(&TcpSocketEpoll::_checkTimeout, socket_ptr));
#endif

            socket_ptr->set_conn_status(CLIENT_CONNECTING);

            int status = socket_ptr->conn_status();
            int test_times = 10;
            while (status == CLIENT_CONNECTING && test_times > 0)
            {
				status = sock_connect(socket_ptr->socket(), ip, port);
                if (status == CLIENT_UNCONNECTED)
                {
                    status = CLIENT_CONNECTING;
                    test_times--;
                }
            }

            if (test_times <= 0)
            {
                QLOG_DEBUG("connected failed :(");
                status = CLIENT_UNCONNECTED;
				return false;
            }
            
			QLOG_DEBUG("connected :)");

			//this->_poll_thread = std::thread(std::bind(&TcpClient::run, this));
			/* 消息队列 */
			//this->_message_thread = std::thread(std::bind(&MessageWorker::run, this->messageWorker()));

			this->onConnect(socket_ptr->socket(), serverType);
			return true;
		}

		void TcpClient::setTimeout(uint32_t timeout_seconds)
		{
			this->_timeout = timeout_seconds;
		}
	}
}
