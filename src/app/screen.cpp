#include "app/screen.h"

void screen::loop()
{
    while(!_states.empty()) {
        auto const& loop = _states.front()->loop;
        if(loop != nullptr)
            loop();
        
        if(!_states.front()->done())
            break;
        pop_front();
    }
    
    get_action().update();
}

void screen::push_front(screen_state* state) {
    _states.emplace_front(state);
    if(_states.size() == 1) {
        _states.front()->on_enter();
    }
}

void screen::push_back(screen_state* state) {
    _states.emplace_back(state);
    if(_states.size() == 1) {
        _states.front()->on_enter();
    }
}

void screen::pop_front() {
    if(_states.size() == 0)
        return;
    
    _states.front()->on_leave();
    _states.pop_front();

    if(!_states.empty()) {
        _states.front()->on_enter();
    }
}

void screen::clear_state() {
    if(_states.size() == 0)
        return;
    
    _states.front()->on_leave();
    _states.pop_front();

    while(!_states.empty()){
        _states.front()->on_skip();
        _states.pop_front();
    }
}
