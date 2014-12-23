#include "action_support/action_animation.h"
#include "sg/transform.h"

using namespace act;

action_animation::action_animation(com::animation_clip::ptr const& clip,
                                   com::animation const* anim,
                                   timer const& timer_)
: _clip(clip), _animation(anim), _timer(timer_) {
    _channels.resize(_animation->transforms().size(), nullptr);
    _clip->get_channels(_animation->names(), _channels);
}

void action_animation::update() {
    typedef interpolator_linear<vector3f> linear_t;
    typedef interpolator_linear<float> linearf_t;
    typedef interpolator_slerp<quaternionf> slerp_t;
    action::update();

    auto elapsed = _timer.current_time() - _start;
    auto normalized = fmod(elapsed, _loop) / _loop;
    
    auto setup_it = _animation->setup_poses().begin();
    auto go_it = _animation->transforms().begin();
    auto channel_it = _channels.begin();
    for (; channel_it != _channels.end(); ++go_it, ++channel_it, ++setup_it) {
        if (*channel_it == nullptr)
            continue;
        
        if ((*channel_it)->rotate) {
            (*go_it)->set_rotate(setup_it->rotate
                                 * (*channel_it)->rotate->interpolate_in_frame<slerp_t, void>(normalized));
        }
        if ((*channel_it)->translate) {
            (*go_it)->set_translate(setup_it->translate +
                                    (*channel_it)->translate->interpolate_in_frame<linear_t, void>(normalized));
        }
        if ((*channel_it)->scale) {
            // TODO/FIXME
            //(*go_it)->set_scale(//setup_it->scale +
            //                    (*channel_it)->scale->interpolate<linear_t>(normalized));
        }
        
        (*go_it)->mark_dirty();
        
        // TODO: more channels and more types of interpolation
    }
}

bool action_animation::done() const {
    return false && action::done();
}

void action_animation::on_start() {
    _start = _timer.current_time();
    return action::on_start();
}