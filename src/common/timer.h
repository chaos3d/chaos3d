#ifndef _CHAOS3D_TIMER_H
#define _CHAOS3D_TIMER_H

#ifdef __APPLE__
#include <mach/mach_time.h>
#include <stdint.h>
#endif
#include <ctime>

#include "common/utility.h"
#include "common/singleton.h"

class timer {
public:
    typedef uint32_t frame_t;
    typedef double time_t;
    
#ifdef __APPLE__
    typedef uint64_t tick_t;
#else
    typedef clock_t tick_t;
#endif
    
    // ticking using the given time
    class ticker_realtime {
    public:
        ticker_realtime(tick_t now = current())
        : _last_tick(now)
        {}
        
        tick_t ticking(tick_t now) {
            tick_t delta = now - _last_tick;
            _last_tick = now;
            return delta;
        }
        
        tick_t operator() (tick_t now) {
            return ticking(now);
        }
        
    private:
        tick_t _last_tick;
    };
    
    // ticking using the fixed ticks
    class ticker_fixed {
    public:
        ticker_fixed(frame_t frames = 30)
        : _fixed_tick(_tick_per_second / (frames == 0 ? 30 : frames))
        {}
        
        tick_t ticking(tick_t now) {
            return _fixed_tick;
        }
        
        tick_t operator() (tick_t now) {
            return ticking(now);
        }
        
    private:
        tick_t _fixed_tick;
    };

public:
    // tick delta, return the delta frames
    frame_t tick(tick_t delta) {
        frame_t previous = _current;
        _previous_tick = _tick;
        _tick += delta;
        while (_tick - _last_frame_tick > _frame_rate) {
            _last_frame_tick += _frame_rate;
            ++ _current;
        }
        return _current - previous;
    }
    
    void set_frame_rate(frame_t frames) {
        _frame_rate = _tick_per_second / frames;
    }
    
    // current tracking frame
    frame_t current_frame() const {
        return _current;
    }
    
    // current tracking time
    time_t current_time() const {
#ifdef __APPLE__
        return (time_t)_tick / _tick_per_second;
#else
        return (time_t)_tick / CLOCKS_PER_SEC;
#endif
    }
    
    time_t recent_delta() const {
#ifdef __APPLE__
        return (time_t)(_tick - _previous_tick) / _tick_per_second;
#else
        return (time_t)(_tick - _previous_tick) / CLOCKS_PER_SEC;
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
#ifdef __APPLE__
    static time_t tick_per_second() {
        mach_timebase_info_data_t timebase_info;
        (void) mach_timebase_info(&timebase_info);
        return timebase_info.denom*1000000000L / timebase_info.numer;
    }

    static const time_t _tick_per_second;
#endif
    
private:
    frame_t _current = 0;
    tick_t _tick = 0, _last_frame_tick = 0, _previous_tick = 0;
    tick_t _frame_rate = _tick_per_second / 30;
};

class global_timer_base : public timer, public singleton<global_timer_base> {
public:
    virtual void update() = 0;
};

template<class T>
class global_timer : public global_timer_base {
public:
    template<class... Args>
    global_timer(Args&&... args)
    : _ticker(std::forward<Args>(args)...)
    {}
    
    virtual void update() override {
        tick(_ticker(current()));
    }
private:
    T _ticker;
};

template<class T, class... Args>
global_timer<T>* make_global_timer(Args&&... args) {
    return new global_timer<T>(std::forward<Args>(args)...);
}
#endif