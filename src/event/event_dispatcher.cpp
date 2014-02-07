#include "event/event_dispatcher.h"

bool event_dispatcher::register_listener(std::initializer_list<std::type_index> types,
                                         event_listener* listenr) {
    for(auto& it : types) {
        _listeners.emplace(it, listenr->get<event_listener>());
    }
    
    for(auto& it : types) {
        auto range = _listeners.equal_range(it);
        for(auto it = range.first; it != range.second; ) {
            auto del = it ++;
            if(del->second->expired()) {
                _listeners.erase(del);
            }
        }
    }
    return true;
}

bool event_dispatcher::unregister_listener(event_listener* listener) {
    for(auto it = _listeners.begin(); it != _listeners.end();) {
        auto del = it ++;
        if(del->second->raw_pointer() == listener) {
            _listeners.erase(del);
        }
    }
    return true;
}

void event_dispatcher::dispatch(event const& evt) {
    auto range = _listeners.equal_range(typeid(*&evt));
    for(auto it = range.first; it != range.second; ++it) {
        if(!it->second->expired()) {
            auto listener = dynamic_cast<event_listener*>(it->second->raw_pointer());
            if(listener)
                listener->on_event(evt);
        }
    }
}

