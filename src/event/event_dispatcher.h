#ifndef _EVENT_DISPATCHER_H
#define _EVENT_DISPATCHER_H

#include <type_traits>
#include <typeindex>
#include <memory>
#include <unordered_map>
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

// dispatcher
class event_dispatcher {
public:
    typedef referenced_count::weak_ref_ctrl<event_listener>::weak_ptr listener_ptr;
    typedef std::unordered_multimap<std::type_index, listener_ptr> listeners_t;
    
public:
    virtual ~event_dispatcher() {};
    virtual bool register_listener(std::initializer_list<std::type_index> ,
                                   event_listener*);
    
    virtual bool unregister_listener(event_listener*);
    
    template<class... Args>
    bool register_listener(event_listener* listener) {
        return register_listener({std::type_index(typeid(Args))...},
                                 listener);
    }
    
protected:
    void dispatch(event const&);

private:
    listeners_t _listeners;
};

#endif