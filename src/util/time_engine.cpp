#include "qing/util/time_engine.h"

#ifdef _WIN32
    #include <sys/timeb.h>
    #include <sys/types.h>
    #include <windows.h>
#else
    #include <sys/time.h>
	#include <sys/times.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <sys/types.h>
	#include <unistd.h>
#endif




qing::Thread::Thread() 
{

}


qing::Thread::~Thread()
{

}

int qing::Thread::cancel(void)
{
    //pthread_cancel(this->pid);
    return 0;
}

void qing::ETimer::open_sec(uint32_t interval_sec, std::function<void()> fp, uint64_t now_clock)
{
	_interval = (uint64_t)interval_sec * 1000000;
    _leftsecs = _interval;
    _startclock = now_clock;
    _fp = fp;
}

void qing::ETimer::open_milsec(uint32_t interval_milsec, std::function<void()> fp, uint64_t now_clock)
{
	_interval = (uint64_t)interval_milsec * 1000;
    _leftsecs = _interval;
    _startclock = now_clock;
    _fp = fp;
}



qing::TimerThread::TimerThread()
:_cur_timer_id(0)
, time_zone_sec_(0)
{
    Init();
}

qing::TimerThread::~TimerThread()
{
    _timers1.clear();
    _timers2.clear();
}

void qing::TimerThread::Register(ETimer& timer)
{
	do {
			{
				std::unique_lock<std::recursive_mutex> lock(_timers2_mutex, std::defer_lock);
				if (lock.try_lock())
				{
					_timers2.insert(std::make_pair(timer.timer_id, timer));
					break;
				}
			}

			{
				std::unique_lock<std::recursive_mutex> lock(_timers1_mutex, std::defer_lock);
				if (lock.try_lock())
				{
					_timers1.insert(std::make_pair(timer.timer_id, timer));
					break;
				}
			}		
	} while (true);
}

int qing::TimerThread::Register(bool run_once, int interval_time, std::function<void()> fp)
{
    ETimer timer;
    timer.run_once = run_once;
    timer.timer_id = ++_cur_timer_id;
    auto now_clock = get_clock();
    timer.open_sec(interval_time, fp, now_clock);
    Register(timer);
    return _cur_timer_id;
}

int qing::TimerThread::RegisterByMilSec(bool run_once, int interval_time, std::function<void()> fp)
{
    ETimer timer;
    timer.run_once = run_once;
    timer.timer_id = ++_cur_timer_id;
    auto now_clock = get_clock();
    timer.open_milsec(interval_time, fp, now_clock);
    Register(timer);
    return _cur_timer_id;
}

void qing::TimerThread::UnRegister(int timer_id)
{
	{
		std::unique_lock<std::recursive_mutex> lock(_timers2_mutex);

		auto it2 = _timers2.find(timer_id);
		if (it2 != _timers2.end())
		{
			it2->second.need_delete = true;
			return;
		}

	}

	{
		std::unique_lock<std::recursive_mutex> lock(_timers1_mutex);

		auto it1 = _timers1.find(timer_id);
		if (it1 != _timers1.end())
		{
			it1->second.need_delete = true;
			return;
		}
	}

}

void qing::TimerThread::Update()
{
	{
		std::unique_lock<std::recursive_mutex> lock(_timers1_mutex);

		for (auto it = _timers1.begin(); it != _timers1.end();)
		{
			if (it->second.need_delete)
			{
				_timers1.erase(it++);
				continue;
			}

			auto& timer = it->second;
			auto now_clock = get_clock();
			auto interval_time = get_interval_macro_secs(now_clock, timer._startclock);
			if (timer._leftsecs <= interval_time)
			{
				timer._fp();
				timer._leftsecs = timer._interval;
				timer._startclock = get_clock();
				if (timer.run_once)
				{
					_timers1.erase(it++);
					continue;
				}
			}
			it++;
		}
	}

	{
		std::unique_lock<std::recursive_mutex> lock(_timers2_mutex);

		for (auto it = _timers2.begin(); it != _timers2.end();)
		{
			if (it->second.need_delete)
			{
				_timers2.erase(it++);
				continue;
			}

			auto& timer = it->second;
			auto now_clock = get_clock();
			auto interval_time = get_interval_macro_secs(now_clock, timer._startclock);
			if (timer._leftsecs <= interval_time)
			{
				timer._fp();
				timer._leftsecs = timer._interval;
				timer._startclock = get_clock();
				if (timer.run_once)
				{
					_timers2.erase(it++);
					continue;
				}
			}
			it++;
		}
	}
}


uint64_t qing::TimerThread::get_systime_clock() /// us
{

#ifdef _WIN32
    struct _timeb tb;
    _ftime(&tb);
    return (tb.time * 1000 + tb.millitm)*1000;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000*1000 + tv.tv_usec;
#endif
}

uint64_t qing::TimerThread::get_clock()
{
#ifdef _WIN32
    LARGE_INTEGER now_clock;
    QueryPerformanceCounter(&now_clock);
	return (uint64_t)(now_clock.QuadPart);
#else
	// clock for linux 
	struct tms tmp;
	return (uint64_t)times(&tmp);
#endif // _WIN32
	
}

uint64_t qing::TimerThread::get_interval_macro_secs(uint64_t end_clock, uint64_t start_clock)
{
#ifdef _WIN32
    LARGE_INTEGER count_tick;
    QueryPerformanceFrequency(&count_tick);
	return (uint64_t)((end_clock - start_clock) * 1000000 / (uint64_t)(count_tick.QuadPart));
#else
	
	uint64_t clocks_per_sec = sysconf(_SC_CLK_TCK);
	uint64_t ret = (uint64_t)((end_clock - start_clock) * 1000000 / clocks_per_sec);
	return ret;
#endif // _WIN32

}

uint64_t qing::TimerThread::time_sec()
{
    return time_millisec()/1000;
}


uint64_t qing::TimerThread::time_millisec()
{
    return get_systime_clock() / 1000;
}


bool qing::TimerThread::IfTriggerLocalTimer(int last_trigger_time, int timer_period, int timer_trigger_at)
{
	uint64_t local_sec = qing::TimerThread::time_sec() + time_zone_sec();

	uint64_t diff_count = local_sec / timer_period -
        (last_trigger_time+time_zone_sec()) / timer_period;
    if (diff_count <= 0)
    {
        return false;
    }
    else if (diff_count == 1)
    {
        return timer_trigger_at <= local_sec % timer_period;
    }
    else
    {
        return true;
    }
}

void qing::TimerThread::Init()
{
    time_t t_now = ::time(nullptr);
    struct tm tm_l, tm_g;
    localtime_r(&t_now, &tm_l);
    gmtime_r(&t_now, &tm_g);
    time_t ts_l = mktime(&tm_l);
    time_t ts_g = mktime(&tm_g);

    int time_zone = static_cast<uint32_t>((ts_l - ts_g) / 3600);

    // 不知道到底时区怎么分的, 有的最大+12 有的+13 还有+14????
    if (!(time_zone > 15 || time_zone < -12))
    {
        time_zone_sec_ = time_zone * 3600;
    }
    else
    {
        time_zone_sec_ = 8 * 3600;
    }
}

void qing::TimerThread::fun_time_test_begin(std::string name)
{
    _fun_time_test[name]._begin = get_clock();
}

void qing::TimerThread::fun_time_test_end(std::string name)
{
    _fun_time_test[name]._end = get_clock();
    _fun_time_test[name]._sum += (_fun_time_test[name]._end - _fun_time_test[name]._begin);
}

void qing::TimerThread::print_all_fun_time()
{
    auto itr = _fun_time_test.begin();
    auto itr_end = _fun_time_test.end();
    for (; itr != itr_end;itr++)
    {
        printf("%s = %d\n", itr->first.c_str(), itr->second._sum);
    }
}
