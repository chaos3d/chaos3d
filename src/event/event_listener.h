#ifndef _CHAOS3D_EVENT_EVENT_LISTENER_H
#define _CHAOS3D_EVENT_EVENT_LISTENER_H

#include <memory>
#include "common/referenced_count.h"

// base class for all events
class event {
public:
    virtual ~event() {};
};

// listener interface: weak referenced
struct event_listener : virtual public referenced_count {
    typedef std::unique_ptr<event_listener> ptr;
    typedef std::unique_ptr<const event_listener> const_ptr;
    
    virtual ~event_listener() {};
    virtual bool on_event(event const&) = 0;
};

template<class F>
class listener_functor : public event_listener {
public:
    listener_functor(F&& f) : _functor(std::forward<F>(f))
    {}
    
protected:
    virtual bool on_event(event const&evt) override {
        return _functor(evt);
    }
    
private:
    typename std::remove_cv<F>::type _functor;
};

template<class F>
event_listener::ptr make_listener(F&& f) {
    return event_listener::ptr(new listener_functor<F>(std::forward<F>(f)));
}

#endif