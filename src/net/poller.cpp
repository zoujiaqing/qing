/**
*  COPYRIGHT NOTICE
*  Copyright (c) 2015, Brian Zou All rights reserved.
*
*  @file poller.cpp
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
		Poller::Poller()
		{
			/* IOCP */
#ifdef HAVE_WINDOWS
			WSADATA wsa;
			if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
			{
				QLOG_ERROR("WSAStartup ERROR.");
			}

			this->_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
			if (!this->handle())
			{
				QLOG_ERROR("CreateIoCompletionPort() Error.");
			}
#endif

			/* epoll */
#ifdef HAVE_EPOLL

#ifdef EPOLL_CLOEXEC
			this->_handle = epoll_create1(EPOLL_CLOEXEC);
#else
			this->_handle = epoll_create(100);
#endif
			if (this->_handle == -1)
			{
				QLOG_ERROR("epoll_create() Error.");
			}
#endif

			/* kqueue */
#ifdef HAVE_KQUEUE
			this->_handle = kqueue();
			if (this->_handle == -1)
			{
				QLOG_ERROR("kqueue() Error.");
			}
#endif
		}

		Poller::~Poller()
		{
#ifdef HAVE_IOCP
			bool ret = CloseHandle(this->handle());
			if (!ret)
			{
				QING_TRACE();
			}

			WSACleanup();
#else
			::close(this->_handle);
#endif
		}

#ifdef HAVE_IOCP
		bool Poller::poll(uint32_t timeout)
		{

			DWORD countBytes = 0;
			ULONG_PTR completionKey = NULL;
			LPOVERLAPPED overlapped = NULL;

			bool ret = GetQueuedCompletionStatus(this->_handle, &countBytes, &completionKey, &overlapped, timeout < 0 ? INFINITE : timeout);

			if (!ret && !overlapped)
			{
				return false;
			}

			IO_DATA* ioData = (IO_DATA*)overlapped;
			ioData->bytes = countBytes;

			if (ioData->type == HANDLE_COMPLETE)
			{
				return true;
			}

			BaseSocket* baseSocket = this->_socketPool[ioData->socket];

			switch (ioData->type)
			{
				case IO_TYPE::HANDLE_ACCEPT:
				case IO_TYPE::HANDLE_READ:
				{
					baseSocket->onRead();
					break;
				}
				case IO_TYPE::HANDLE_WRITE:
				{
					baseSocket->onWrite();
					break;
				}
				default:
				{
					QING_TRACE();
					break;
				}
			}

			return true;
		}
#endif

#ifdef HAVE_EPOLL
		/* for Linux */
		bool Poller::poll(uint32_t timeout)
		{
			uint32_t result = epoll_wait(this->_handle, this->_events, MAX_EVENT_SIZE, timeout < 0 ? INFINITE : timeout);
			if (result <  0)
			{
				if (errno == EINTR)
				{
					return true;
				}
				else
				{
					QLOG_ERROR("epoll_wait end,errno=%d", errno);//
					return false;
				}
			}

			if (result == 0) return true;

			for(uint32_t i=0; i<result; i++)
			{
				BaseSocket* baseSocket = (EventHandler*)this->_events[i].data.ptr;
				uint32_t events = activeEvs_[i].events;

				if (baseSocket == nullptr)
				{
					QLOG_WARN("eventHandler is nullptr.");
					continue;
				}

				if (events & EPOLLIN)
				{
					BaseSocket* baseSocket->onRead();
					continue;
				}

				if (events & EPOLLOUT)
				{
					BaseSocket* baseSocket->onWrite();
					continue;
				}
			}

			return true;
		}
#endif

#ifdef HAVE_KQUEUE
		/* for FreeBSD */
		bool Poller::poll(uint32_t timeout)
		{
			struct timespec t = { timeout, 0 };
			uint32_t result = kevent(this->_handle, NULL, 0, this->_events, MAX_EVENT_SIZE, &t);

			if (result == -1)
			{
				QLOG_ERROR("kevent error %d.", errno);
				return false;
			}

			if (result == 0)
			{
				return true;
			}

			for (uint32_t i = 0; i < result; i++)
			{
				BaseSocket* baseSocket = (BaseSocket*)this->_events[i].udata;

				if (eventHandler == nullptr)
				{
					QLOG_WARN("eventHandler is nullptr.");
					continue;
				}

				if (this->_events[i].flags & EV_EOF || this->_events[i].flags & EV_ERROR)
				{
					baseSocket->onRead();
				}

				if (this->_events[i].filter == EVFILT_READ)
				{
					baseSocket->onRead();
					continue;
				}

				if (this->_events[i].filter == EVFILT_WRITE)
				{
					baseSocket->onWrite();
					continue;
				}
			}

			return true;
		}
#endif

		/*
		add BaseSocket*
		*/
		bool Poller::add(BaseSocket* BaseSocket, IO_EVENT_TYPE e)
		{
			this->_socketPool[BaseSocket->socket()] = BaseSocket;

#ifdef HAVE_IOCP
			if (!CreateIoCompletionPort((HANDLE)BaseSocket->socket(), this->handle(), 0, 0))
			{
				QLOG_ERROR("CreateIoCompletionPort Error %d", WSAGetLastError());
				return false;
			}
#endif

#ifdef HAVE_EPOLL
			if (!eventHandler)
			{
				QLOG_ERROR("eventHandler error \n");
				return false;
			}

			struct epoll_event ev;
			memset(&ev, 0, sizeof(ev));

			ev.events = this->convertEventMask(EVENT_READ);
			ev.data.u64 = 0;
			ev.data.fd = socket;
			ev.data.ptr = (void*)BaseSocket;

			if (epoll_ctl(this->_handle, EPOLL_CTL_ADD, socket, &ev) < 0)
			{
				QLOG_ERROR("epoll_ctl EPOLL_CTL_ADD error!");
				return false;
			}
#endif

#ifdef HAVE_KQUEUE
			struct kevent ev[1];
			EV_SET(&ev[0], socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, (void*)BaseSocket);
			if (kevent(this->_handle, ev, 1, NULL, 0, NULL) < 0)
			{
				QLOG_ERROR("kevent EV_SET error, errno %d.", errno);
				return false;
			}
#endif
			return true;
		}

		/* 方法在 IOCP 环境下直接返回 true */
		bool Poller::modify(BaseSocket* BaseSocket, IO_EVENT_TYPE e)
		{
#ifdef HAVE_EPOLL
			struct epoll_event ev;
			memset(&ev, 0, sizeof(ev));

			ev.events = this->convertEventMask(e);
			ev.data.ptr = (void*)BaseSocket;

			// epoll_fd, type, fd, event
			if (epoll_ctl(this->_handle, EPOLL_CTL_MOD, socket, &ev) < 0)
			{
				QLOG_ERROR("epoll_ctl EPOLL_CTL_MOD error, errno %d.", errno);
				return false;
			}
#endif

#ifdef HAVE_KQUEUE
			auto eventHandler = SocketMgr()->GetHandler(BaseSocket->socket());
			if (nullptr == eventHandler)
			{
				QLOG_WARN("unknow handler, socket %d.", BaseSocket->socket());
				return false;
			}

			struct kevent ev[1];
			EV_SET(&ev[0], socket, this->convertEventMask(e), EV_ADD | EV_ENABLE, 0, 0, (void*)eventHandler);
			if (kevent(this->_handle, ev, 1, NULL, 0, NULL) < 0)
			{
				QLOG_ERROR("kevent EV_SET error, errno %d.", errno);
				return false;
			}

			eventHandler->SetCurrentEvent(e);
#endif
			return true;
		}

		bool Poller::remove(BaseSocket* BaseSocket, IO_EVENT_TYPE e)
		{
#ifdef HAVE_EPOLL
			uint32_t result = epoll_ctl(this->_handle, EPOLL_CTL_DEL, socket, 0);
			if(result < 0)
			{
				QLOG_ERROR("epoll_ctl remove error %d", errno);
				return false;
			}
#endif

#ifdef HAVE_KQUEUE
			struct kevent ev;
			memset(&ev, 0, sizeof(ev));

			EV_SET(&ev, socket, this->convertEventMask(e), EV_DELETE | EV_DISABLE, 0, 0, 0);
			uint32_t result = kevent(this->_handle, &ev, 1, 0, 0, 0);
			if (result < 0)
			{
				QLOG_ERROR("kevent remove error %d", errno);
				return false;
			}
#endif
			return true;
		}

#ifdef HAVE_EPOLL
		uint32_t Poller::convertEventMask(IO_EVENT_TYPE e)
		{
			uint32_t op = 0;
			if (e & EVENT_READ) op |= (EPOLLIN | EPOLLONESHOT);
			if (e & EVENT_WRITE) op |= (EPOLLOUT | EPOLLONESHOT);
			if (e & EVENT_ERROR) op |= EPOLLERR;
			return op;
		}
#endif

#ifdef HAVE_KQUEUE

		uint16_t Poller::convertEventMask(IO_EVENT_TYPE e)
		{
			uint16_t op = 0;

			if (e & EVENT_READ)		op |= EVFILT_READ;
			if (e & EVENT_WRITE)	op |= EVFILT_WRITE;

			return op;
		}
#endif
	}
}
