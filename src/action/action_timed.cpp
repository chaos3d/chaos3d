#include "action/action_timed.h"

void action_timed::update() {
    time_t off = _timer.current_time() - _start;
    auto it = _actions.begin();
    while (!_actions.empty() && it->first <= off){
        push(it->second);
        _actions.pop_front();
        it = _actions.begin();
    }
    
    action::update();
}

void action_timed::on_start() {
    action::on_start();
    
    _actions.sort([] (timed_action const& lhs, timed_action const& rhs) {
        return lhs.first < rhs.first;
    });
    
    _start = _timer.current_time();
}