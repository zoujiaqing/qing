/**
*  COPYRIGHT NOTICE
*  Copyright (c) 2015, Brian Zou All rights reserved.
*
*  @file poller.hpp
*  @brief ...
*
*  Asynchronous network library
*
*  @author Brian Zou (zoujiaqing@gmail.com)
*  @date   2015/12/26
*/

#pragma once

namespace qing
{
	namespace net
	{
        class Poller : public std::enable_shared_from_this<Poller>
        {
        public:
            Poller();

            ~Poller();

            bool poll(uint32_t timeout);

			bool add(BaseSocket* baseSocket, IO_EVENT_TYPE e);
			bool modify(BaseSocket* baseSocket, IO_EVENT_TYPE e);
			bool remove(BaseSocket* baseSocket, IO_EVENT_TYPE e);

			handle_t handle() { return this->_handle; };

#ifdef HAVE_EPOLL
			uint32_t convertEventMask(uint32_t e);
#elif defined(HAVE_KQUEUE)
			uint16_t convertEventMask(uint32_t e);
#endif

		protected:
			handle_t _handle;
			std::unordered_map < socket_t, BaseSocket* > _socketPool;
		};
	}
}
