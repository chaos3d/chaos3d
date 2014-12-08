#ifndef _CHAOS3D_ACTION_TIMED_H
#define _CHAOS3D_ACTION_TIMED_H

#include <forward_list>
#include "action/action.h"
#include "common/timer.h"

// action to wait for a certain amount of time/frame
class action_timer : public action {
public:
    typedef timer::time_t time_t;

public:
    action_timer(time_t duration,
                 timer const& t = global_timer_base::instance())
    : _duration(duration), _timer(t)
    {}

    static action::ptr wait(time_t duration) {
        return action::ptr(new action_timer(duration));
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

class action_frame : public action {
    typedef timer::frame_t frame_t;
public:
    action_frame(frame_t duration,
                 timer const& t = global_timer_base::instance())
    : _duration(duration), _timer(t) {
        
    }

    // yield a certain amount of frames
    // alias to wait, just for readability
    static action::ptr yield(frame_t frame = 0) {
        return action::ptr(new action_frame(frame));
    }
    
protected:
    virtual void on_start() {
        action::on_start();
        _start = _timer.current_frame();
    };
    
    virtual bool done() const {
        return _timer.current_frame() - _start > _duration && action::done();
    }
    
private:
    frame_t _start = 0;
    frame_t _duration = 0;
    timer const& _timer;
};


// action to trigger other actions at a certain time
class action_timed : public action {
public:
    typedef timer::time_t time_t;
    typedef std::pair<time_t, ptr> timed_action;
    typedef std::forward_list<timed_action> actions_t;
    
public:
    action_timed(actions_t&& actions)
    : _actions(std::move(actions))
    {}
    
    action_timed* add(time_t t, ptr&& a) {
        _actions.emplace_front(std::piecewise_construct,
                               std::forward_as_tuple(t),
                               std::forward_as_tuple(std::move(a)));
        return this;
    };
    
    static ptr timed(actions_t&& actions) {
        return ptr(new action_timed(std::move(actions)));
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