#include "action_support/action_animation.h"
#include "com/anim/skeleton_transform.h"
#include "sg/transform.h"

using namespace act;

struct action_animation::internal {
    com::skeleton_animation_clip::ptr clip; // only one animation for now
    std::vector<com::transform*> transforms;
};

action_animation::action_animation(timer const& timer_)
: _timer(timer_) {
}

void action_animation::update() {
    action::update();

    auto elapsed = _timer.current_time() - _start;
    auto normalized = fmod(elapsed, _loop) / _loop;
    _internal->clip->apply((_duration > 0.f && elapsed >= _duration) ? 1.f
                           : normalized < 0.f ? 0.f
                           : normalized,
                           _internal->transforms);
}

bool action_animation::done() const {
    return action::done();
}

void action_animation::on_start() {
    _start = _timer.current_time();
    return action::on_start();
}