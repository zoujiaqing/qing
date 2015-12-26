#pragma once
#include <map>
#include <functional>
#include <qing/define.hpp>
#include <time.h>
#include <mutex>


#ifdef _WIN32
#ifndef snprintf
#define snprintf sprintf_s
#endif
#define localtime_r(a,b) localtime_s(b, a)
#define gmtime_r(a,b) gmtime_s(b, a)
#endif // _WIN32

namespace qing
{

    class Thread
    {
    public:
        enum THREADSTATE
        {
            IDLE,
            WORK,
            BUSY,
        };

    public:

        Thread();

        virtual ~Thread();

        virtual void Update() = 0;

        virtual int cancel(void);

    protected:

        THREADSTATE _thread_state;
    };


    struct ETimer
    {

        void *_args;
        void(*_callback)();
        uint64_t _interval;          // us
        uint64_t _leftsecs;          // us
		uint64_t _startclock;        // us
        int timer_id;
        bool run_once;
        bool need_delete = false;
        std::function<void()> _fp;
        // interval s:
		void open_sec(uint32_t interval_sec, std::function<void()> fp, uint64_t now_clock);

        // ms
		void open_milsec(uint32_t interval_milsec, std::function<void()> fp, uint64_t now_clock);

        bool operator < (ETimer _timer)
        {
            return _timer._leftsecs < this->_leftsecs;
        }

        bool operator == (ETimer _timer)
        {
            return _timer._leftsecs == this->_leftsecs;
        }

    };

    class TimerThread : public Thread
    {

    public:
        TimerThread();
        virtual void Update();

        virtual ~TimerThread();


        // interval_time: s
        int Register(bool run_once, int interval_time, std::function<void()> fp);

        // interval_time: ms
        int RegisterByMilSec(bool run_once, int interval_time, std::function<void()> fp);

        void UnRegister(int timer_id);

        int LocalDayDiff(int ts1, int ts2) 
        {
            int day_diff = ts1 / (24 * 60 * 60) - ts2 / (24 * 60 * 60);
            return day_diff;
        }
        bool IfTriggerLocalTimer(int last_trigger_time, int timer_period, int timer_trigger_at);

		uint64_t get_systime_clock();
		uint64_t get_clock();
		uint64_t get_interval_macro_secs(uint64_t end_clock, uint64_t start_clock);
        // s
		uint64_t time_sec();
        // ms
		uint64_t time_millisec();
        // 
       uint32_t time_zone_sec(){return time_zone_sec_;}

        void fun_time_test_begin(std::string name);
        void fun_time_test_end(std::string name);
        void print_all_fun_time();

    private:
        void Init();
        void Register(ETimer& timer);
		std::recursive_mutex			  _timers1_mutex;
		std::recursive_mutex			  _timers2_mutex;
        std::map<int, ETimer> _timers1;
        std::map<int, ETimer> _timers2;
        int _cur_timer_id;
       uint32_t time_zone_sec_;

        struct SFunTime
        {
			uint64_t _begin = 0;
			uint64_t _end = 0;
			uint64_t _sum = 0;
        };
        std::map<std::string, SFunTime> _fun_time_test;
    };

}
