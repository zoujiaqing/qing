/**
*  COPYRIGHT NOTICE
*  Copyright (c) 2015, Brian Zou All rights reserved.
*
*  @file util.hpp
*  @brief ...
*
*  Asynchronous network library
*
*  @author Brian Zou (zoujiaqing@gmail.com)
*  @date   2015/12/26
*/

#pragma once

#include <qing/net/define.hpp>

#ifdef HAVE_WINDOWS
#include <ws2tcpip.h>
#else
#include<netdb.h>
#endif

namespace qing
{
	namespace net
	{
		class Util
		{
		public:

			static bool setNonblocking(socket_t socket);
			static socket_t getNewSocket();
			static std::string cenvertBytesToSize(uint32_t size);
			static uint32_t getHostByName(const std::string& host);
		};
	}
} // namespace qing
