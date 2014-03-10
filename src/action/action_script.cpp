#include "action/action_script.h"

bool action_script::done() const {
    // error is considered to be done
    return !_coroutine.is_yielded() && action::done();
}

void action_script::on_start() {
    action::on_start();
    
    _yielded.reset(std::get<1>(_coroutine.resume<yieldable*>()));
}

void action_script::update() {
    action::update();
    
    if (!_yielded || _yielded->is_done()) {
        _yielded.reset(std::get<1>(_coroutine.resume<yieldable*>()));
    } else if (_yielded) {
        _yielded->update();
    }
}
