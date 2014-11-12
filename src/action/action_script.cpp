#include "action/action_script.h"
#include "common/timer.h"

bool action_script::done() const {
    // error is considered to be done
    return (!_yielded || _yielded->done()) && !_coroutine.is_yielded() && action::done();
}

void action_script::yield_to(ptr&& act) {
    _yielded.reset(act.release());
    if (_yielded) {
        _yielded->start();
    }
}

void action_script::on_start() {
    action::on_start();
    auto& timer = global_timer_base::instance();
    yield_to(std::get<1>(_coroutine.resume<ptr>(timer.current_time(),
                                                timer.recent_delta())));
}

void action_script::update() {
    action::update();
    
    auto& timer = global_timer_base::instance();
    if (!_yielded || _yielded->done()) {
        yield_to(std::get<1>(_coroutine.resume<ptr>(timer.current_time(),
                                                    timer.recent_delta())));
    } else if (_yielded) {
        _yielded->update();
    }
}
