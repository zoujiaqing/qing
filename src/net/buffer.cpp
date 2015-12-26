/**
*  COPYRIGHT NOTICE
*  Copyright (c) 2015, Brian Zou All rights reserved.
*
*  @file buffer.cpp
*  @brief ...
*
*  Asynchronous network library
*
*  @author Brian Zou (zoujiaqing@gmail.com)
*  @date   2015/12/26
*/

#include <qing/net/define.hpp>
#include <qing/net/buffer.hpp>

namespace qing
{
	namespace net
	{
		Buffer::Buffer()
		{

		}

		Buffer::~Buffer()
		{

		}

		void Buffer::release()
		{
			this->_buffer.clear();
		}

		/* 返回buffer的内存地址 */
		const char* Buffer::buffer() const
		{
			return &this->_buffer[0];
		}

		size_t Buffer::size()
		{
			return this->_buffer.size();
		}

		void Buffer::write(const char* data, uint32_t size)
		{
			if (data == NULL || size == 0)
				return;

			this->_buffer.append(data, size);
		}

		/* 从缓冲的开始部分提取指定大小的数据块 */
		bool Buffer::fetch(char* data, uint32_t size)
		{
			if (size > this->size())
			{
				return false;
			}

			memcpy(data, &this->_buffer[0], size);

			this->_buffer.erase(this->_buffer.begin(), this->_buffer.begin() + size);
			return true;
		}
	}
}
