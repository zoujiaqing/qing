/**
*  COPYRIGHT NOTICE
*  Copyright (c) 2015, Brian Zou All rights reserved.
*
*  @file buffer.hpp
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
		class  Buffer
		{
		public:
			Buffer();

			~Buffer();

			void release();

			/* 返回buffer的内存地址 */
			const char* buffer() const;

			size_t size();

			void write(const char* data, uint32_t size);

			/* 从缓冲的开始部分提取指定大小的数据块 */
			bool fetch(char* data, uint32_t size);

		private:
			// std::vector<char>	_buffer;
			std::string	_buffer;
		};
	}
}
