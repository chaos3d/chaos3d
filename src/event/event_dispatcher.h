#ifndef _EVENT_DISPATCHER_H
#define _EVENT_DISPATCHER_H

#include <type_traits>
#include <typeindex>
#include <memory>
#include <unordered_map>
#include "common/referenced_count.h"

// base class for all events
class event {};

// listener interface: weak referenced
struct event_listener : virtual public referenced_count {
    virtual ~event_listener() {};
    virtual bool on_event(event const&) = 0;
};

// dispatcher
class event_dispatcher {
public:
    typedef referenced_count::weak_ref_ctrl<event_listener>::weak_ptr listener_ptr;
    typedef std::unordered_multimap<std::type_index, listener_ptr> listeners_t;
    
public:
    virtual ~event_dispatcher() {};
    virtual bool register_listener(std::initializer_list<std::type_index> ,
                                   listener_ptr const&);
    
    virtual bool unregister_listener(listener_ptr const&);
    
private:
    listeners_t _listeners;
};

#endif