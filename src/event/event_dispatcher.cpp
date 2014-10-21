#include "event/event_dispatcher.h"
#include "common/log.h"

DEFINE_LOGGER(event_dispatcher, "event");

bool event_dispatcher::register_listener(std::vector<std::type_index> types,
                                         event_listener* listenr) {
    for(auto& it : types) {
        LOG_INFO("register listener (" << std::hex << listenr << ") for type: " << it.name());
        _listeners.emplace(it, listenr->get<event_listener>());
    }
    
    for(auto& it : types) {
        auto range = _listeners.equal_range(it);
        for(auto it = range.first; it != range.second; ) {
            auto del = it ++;
            if(del->second->expired()) {
                LOG_INFO("remove listener (" << std::hex << listenr << ") as it is expired");
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
            LOG_INFO("remove listener (" << std::hex << listener << ")");
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
        } else {
            LOG_DEBUG("skip listener for " << it->first.name() << " as it is expired");
        }
    }
}

