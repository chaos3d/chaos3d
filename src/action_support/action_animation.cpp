#include "action_support/action_animation.h"
#include "sg/transform.h"

using namespace act;

action_animation::action_animation(com::skeleton_animation_clip::ptr const& clip,
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
            auto r = setup_it->rotate + (*channel_it)->rotate->interpolate<linearf_t>(normalized);
            (*go_it)->set_rotate(quaternionf(Eigen::AngleAxisf(r,
                                                               vector3f::UnitZ())));
        }
        if ((*channel_it)->translate) {
            (*go_it)->set_translate(setup_it->translate +
                                    (*channel_it)->translate->interpolate<linear_t>(normalized));
        }
        if ((*channel_it)->scale) {
            //(*go_it)->set_scale(//setup_it->scale +
            //                    (*channel_it)->scale->interpolate<linear_t>(normalized));
        }
        
        (*go_it)->mark_dirty();
        
        // TODO: more channels and more types of interpolation
    }
#if 0
    _clip->apply((_duration > 0.f && elapsed >= _duration) ? 1.f
                 : normalized < 0.f ? 0.f
                 : normalized,
                 _animation->transforms());
#endif
}

bool action_animation::done() const {
    return false && action::done();
}

void action_animation::on_start() {
    _start = _timer.current_time();
    return action::on_start();
}