#include "action/action_script.h"
#include "common/timer.h"

bool action_script::done() const {
    // error is considered to be done
    return !_coroutine.is_yielded() && action::done();
}

void action_script::on_start() {
    action::on_start();
    double delta = global_timer_base::instance().recent_delta();
    _yielded.reset(std::get<1>(_coroutine.resume<yieldable*>(delta)));
}

void action_script::update() {
    action::update();
    
    double delta = global_timer_base::instance().recent_delta();
    if (!_yielded || _yielded->is_done()) {
        _yielded.reset(std::get<1>(_coroutine.resume<yieldable*>(delta)));
    } else if (_yielded) {
        _yielded->update(delta);
    }
}
