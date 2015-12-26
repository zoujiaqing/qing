/**
*  COPYRIGHT NOTICE
*  Copyright (c) 2015, Brian Zou All rights reserved.
*
*  @file address.hpp
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
		class Address
		{

		public:
			Address(uint16_t port);
			Address(const std::string& ip, uint16_t port);
			Address(const struct sockaddr_in& addr);

			uint32_t ip() const;
			uint16_t port() const;

			std::string ipString() const;

			const struct sockaddr_in& sockAddr() const { return _sockAddr; }

		private:
			sockaddr_in		_sockAddr;
			std::string		_ip;
			uint16_t		_port;
		};
	}
}
