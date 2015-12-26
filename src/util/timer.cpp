#include <qing/util/timer.hpp>

#ifndef HAVE_WINDOWS
#include <sys/time.h>
#endif

namespace qing
{
	QTimer * QTimer::Instance()
	{
		static QTimer _pTimer;
		return &_pTimer;
	}

	void QTimer::GetLocalTime(struct tm& stTime)
	{
#ifdef HAVE_WINDOWS
		time_t ltime;
		time(&ltime);
		errno_t err;
		err = localtime_s(&stTime, &ltime);
		if (err)
		{
			printf("exit _localtime64_s failed due to invalid arguments.");
			exit(1);
		}
#endif

#ifdef HAVE_LINUX
		struct timeval tv;
		gettimeofday(&tv, NULL);
		localtime_r(&tv.tv_sec, &stTime);
#endif
	}
}