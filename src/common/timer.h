#ifndef _CHAOS3D_TIMER_H
#define _CHAOS3D_TIMER_H

#ifdef __APPLE__
#include <mach/mach_time.h>
#include <stdint.h>
#endif
#include <ctime>

class timer {
public:
    typedef uint32_t frame_t;
    
#ifdef __APPLE__
    typedef uint64_t tick_t;
#else
    typedef clock_t tick_t;
#endif
    
public:
    // move one frame and delta ticks forward
    void tick_one_frame(tick_t delta) {
        ++ _current;
        _tick += delta;
    }
    
    // current tracking frame
    frame_t current_frame() const {
        return _current;
    }
    
    // current tracking time
    double current_time() const {
#ifdef __APPLE__
        return (double)_tick / _tick_per_second;
#else
        return (double)_tick / CLOCKS_PER_SEC;
#endif
    }
    
    // current absolute time
    static tick_t current() {
#ifdef __APPLE__
		return mach_absolute_time();
#else
		return clock();
#endif
	};

private:
    frame_t _current;
    tick_t _tick;

#ifdef __APPLE__
    static double tick_per_second() {
        mach_timebase_info_data_t timebase_info;
        (void) mach_timebase_info(&timebase_info);
        return timebase_info.denom*1000000000L / timebase_info.numer;
    }

    static const double _tick_per_second;
#endif
};

#endif