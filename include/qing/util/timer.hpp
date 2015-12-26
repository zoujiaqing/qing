#pragma once

#include <qing/define.hpp>
#include <time.h>
#include <sstream>
#include <functional>
#include <chrono>
#include <future>
#include <cstdio>

namespace qing
{
	/*
	example:
	{
		qing::Timer timer;
		timer.createTask(this->_timeout * 1000, true, std::bind(&TcpClient::checkTimeout, this));
	}
	*/
	class Timer
	{
	public:
		template <class callable, class... arguments>
		void createTask(uint32_t after, bool async, callable&& func, arguments&&... args)
		{
			std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(func), std::forward<arguments>(args)...));

			/* 需要异步执行使用多线程 */
			if (async)
			{
				std::thread([after, task]() {
					std::this_thread::sleep_for(std::chrono::milliseconds(after));
					task();
				}).detach();
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(after));
				task();
			}
		}
	};

	class QTimer
	{
	public:
		static QTimer* Instance();

		void GetLocalTime(struct tm& stTime);

	private:
		QTimer			*_pTimer;
	};
}