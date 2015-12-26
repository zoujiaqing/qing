/**
*  COPYRIGHT NOTICE
*  Copyright (c) 2015, Brian Zou All rights reserved.
*
*  @file address.cpp
*  @brief ...
*
*  Asynchronous network library
*
*  @author Brian Zou (zoujiaqing@gmail.com)
*  @date   2015/12/26
*/

#include <qing/net/define.hpp>
#include <qing/net/address.hpp>
#include <qing/net/util.hpp>

namespace qing
{
    namespace net
	{
		Address::Address(uint16_t port)
		{
			memset(&_sockAddr, 0, sizeof(_sockAddr));

			_sockAddr.sin_family = AF_INET;
			_sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
			_sockAddr.sin_port = htons(port);
		}

		Address::Address(const std::string& ip, uint16_t port)
		{
			memset(&_sockAddr, 0, sizeof(_sockAddr));

			_sockAddr.sin_family = AF_INET;
			_sockAddr.sin_port = htons(port);

			if (inet_pton(AF_INET, ip.c_str(), &_sockAddr.sin_addr) <= 0)
			{
				uint32_t ret = Util::getHostByName(ip);
				if (ret != uint32_t(-1))
				{
					_sockAddr.sin_addr.s_addr = ret;
					return;
				}

				QLOG_ERROR("invalid ip %s, use 0.0.0.0 instead", ip.c_str());
				_sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
			}
		}

		Address::Address(const struct sockaddr_in& addr)
		{
			memcpy(&_sockAddr, &addr, sizeof(addr));
		}

		uint32_t Address::ip() const
		{
			return ntohl(_sockAddr.sin_addr.s_addr);
		}

		uint16_t Address::port() const
		{
			return ntohs(_sockAddr.sin_port);
		}

		std::string Address::ipString() const
		{
			char buf[32] = { '\0' };
			inet_ntop(AF_INET, (void*)&_sockAddr.sin_addr, buf, sizeof(buf));

			return std::string(buf, 32);
		}
    }
}
