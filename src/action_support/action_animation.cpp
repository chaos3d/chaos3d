#include "action_support/action_animation.h"
#include "sg/transform.h"

using namespace act;

action_animation::action_animation(com::skeleton_animation_clip::ptr const& clip,
                                   com::animation const* anim,
                                   timer const& timer_)
: _clip(clip), _animation(anim), _timer(timer_) {
}

void action_animation::update() {
    action::update();

    auto elapsed = _timer.current_time() - _start;
    auto normalized = fmod(elapsed, _loop) / _loop;
#if 0
    _clip->apply((_duration > 0.f && elapsed >= _duration) ? 1.f
                 : normalized < 0.f ? 0.f
                 : normalized,
                 _animation->transforms());
#endif
}

bool action_animation::done() const {
    return action::done();
}

void action_animation::on_start() {
    _start = _timer.current_time();
    return action::on_start();
}