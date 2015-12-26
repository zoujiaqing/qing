/**
*  COPYRIGHT NOTICE
*  Copyright (c) 2015, Brian Zou All rights reserved.
*
*  @file util.cpp
*  @brief ...
*
*  Asynchronous network library
*
*  @author Brian Zou (zoujiaqing@gmail.com)
*  @date   2015/12/26
*/

#include <qing/net/define.hpp>
#include <qing/net/util.hpp>

namespace qing
{
    namespace net
    {

		socket_t Util::getNewSocket()
		{
			socket_t socket;
#ifdef HAVE_IOCP
			socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
			socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif

			if (-1 == socket)
			{
				return -1;
			}

			uint32_t i = 1;
			if (-1 == setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&i, sizeof(int)))
			{
				QLOG_ERROR("setsocketopt SO_REUSEADDR error");
				return -1;
			}

			uint32_t sendMax = MAX_SEND_SIZE;
			if (-1 == setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char*)&sendMax, sizeof(sendMax)))
			{
				return -1;
			}

			return socket;
		}

        std::string Util::cenvertBytesToSize(uint32_t size)
        {
            float bytes = static_cast<float>(size);

            float tb = 1099511627776;
            float gb = 1073741824;
            float mb = 1048576;
            float kb = 1024;

            char returnSize[32];

            if (bytes >= tb)
                snprintf(returnSize, 32, "%.2f TB", (float)bytes / tb);
            else if (bytes >= gb && bytes < tb)
                snprintf(returnSize, 32, "%.2f GB", (float)bytes / gb);
            else if (bytes >= mb && bytes < gb)
                snprintf(returnSize, 32, "%.2f MB", (float)bytes / mb);
            else if (bytes >= kb && bytes < mb)
                snprintf(returnSize, 32, "%.2f KB", (float)bytes / kb);
            else if (bytes < kb)
                snprintf(returnSize, 32, "%.2f Bytes", bytes);
            else
                snprintf(returnSize, 32, "%.2f Bytes", bytes);

            return std::string(returnSize);
        }

		uint32_t Util::getHostByName(const std::string& host)
		{
			struct addrinfo hint;
			struct addrinfo *answer;

			memset(&hint, 0, sizeof(hint));
			hint.ai_family = AF_INET;
			hint.ai_socktype = SOCK_STREAM;

			int ret = getaddrinfo(host.c_str(), NULL, &hint, &answer);
			if (ret != 0)
			{
				QLOG_ERROR("getaddrinfo error");
				return uint32_t(-1);
			}

			//we only use first find addr
			for (struct addrinfo* cur = answer; cur != NULL; cur = cur->ai_next)
			{
				return ((struct sockaddr_in*)(cur->ai_addr))->sin_addr.s_addr;
			}

			QLOG_ERROR("getHostByName Error");
			return uint32_t(-1);
		}
    }
} // namespace qing
