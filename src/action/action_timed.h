#ifndef _CHAOS3D_ACTION_TIMED_H
#define _CHAOS3D_ACTION_TIMED_H

#include "action/action.h"
#include "common/timer.h"

// action to wait for a certain amount of time
class action_timer : public action {
public:
    typedef timer::time_t time_t;

public:
    action_timer(time_t duration, timer const& t = global_timer::instance())
    : _duration(duration), _timer(t)
    {}

    static action* wait(time_t duration) {
        return new action_timer(duration);
    }

protected:
    virtual void on_start() {
        action::on_start();
        _start = _timer.current_time();
    };
    
    virtual bool done() const {
        return _timer.current_frame() - _start > _duration && action::done();
    }

private:
    time_t _start = 0;
    time_t _duration = 0;
    timer const& _timer;
};

#endif