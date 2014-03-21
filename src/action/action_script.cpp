#include "action/action_script.h"
#include "common/timer.h"

bool action_script::done() const {
    // error is considered to be done
    return !_coroutine.is_yielded() && action::done();
}

void action_script::on_start() {
    action::on_start();
    auto& timer = global_timer_base::instance();
    _yielded.reset(std::get<1>(_coroutine.resume<yieldable*>(timer.current_time(),
                                                             timer.recent_delta())));
}

void action_script::update() {
    action::update();
    
    auto& timer = global_timer_base::instance();
    if (!_yielded || _yielded->is_done()) {
        _yielded.reset(std::get<1>(_coroutine.resume<yieldable*>(timer.current_time(),
                                                                 timer.recent_delta())));
    } else if (_yielded) {
        _yielded->update(timer.recent_delta());
    }
}
