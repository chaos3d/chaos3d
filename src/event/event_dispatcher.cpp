#include "event/event_dispatcher.h"

bool event_dispatcher::register_listener(std::initializer_list<std::type_index> types,
                                         listener_ptr const& listenr) {
    for(auto& it : types) {
        _listeners.emplace(it, listenr->get());
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

bool event_dispatcher::unregister_listener(listener_ptr const& listener) {
    for(auto it = _listeners.begin(); it != _listeners.end();) {
        auto del = it ++;
        if(del->second->raw_pointer() == listener.get()) {
            _listeners.erase(del);
        }
    }
}
