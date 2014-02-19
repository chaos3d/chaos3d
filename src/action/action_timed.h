#ifndef _CHAOS3D_ACTION_TIMED_H
#define _CHAOS3D_ACTION_TIMED_H

#include <forward_list>
#include "action/action.h"
#include "common/timer.h"

// action to wait for a certain amount of time/frame
template<class T>
class action_timing : public action {
public:
    typedef T time_t;

public:
    action_timing(time_t duration,
                  timer const& t = global_timer_base::instance())
    : _duration(duration), _timer(t)
    {}

    static action* wait(time_t duration) {
        return new action_timing(duration);
    }

protected:
    virtual void on_start() {
        action::on_start();
        _start = _timer.current_time();
    };
    
    virtual bool done() const {
        return _timer.current_time() - _start > _duration && action::done();
    }

private:
    time_t _start = 0;
    time_t _duration = 0;
    timer const& _timer;
};

typedef action_timing<timer::time_t> action_timer;

class action_frame : public action_timing<timer::frame_t> {
public:
    // yield a certain amount of frames
    // alias to wait, just for readability
    static action* yield(time_t frame = 0) {
        return new action_timing<timer::frame_t>(frame);
    }
};


// action to trigger other actions at a certain time
class action_timed : public action {
public:
    typedef timer::time_t time_t;
    typedef std::pair<time_t, action*> timed_action;
    typedef std::forward_list<timed_action> actions_t;
    
public:
    action_timed(std::initializer_list<timed_action> actions)
    : _actions(actions)
    {}
    
    action_timed* add(time_t t, action* a) {
        _actions.emplace_front(t, a);
        return this;
    };
    
    static action_timed* timed(std::initializer_list<timed_action> actions) {
        return new action_timed(actions);
    }
protected:
    virtual void on_start() override;
    //virtual void on_end() override;
    virtual void update() override;
    virtual void on_stop(bool s) override {
        action::on_stop(s);
        assert(started() && !done()); // not implemented
    }
    
    virtual bool done() const {
        return _actions.empty() && action::done();
    }
    
    actions_t const& actions() const {
        return _actions;
    };
    
private:
    time_t _start = 0;
    timer const& _timer = global_timer_base::instance();
    actions_t _actions;
};

#endif