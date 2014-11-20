#ifndef _CHAOS3D_ACTION_TRANSFORM_H
#define _CHAOS3D_ACTION_TRANSFORM_H

#include "action/action_keyframe.h"
#include "sg/transform.h"

namespace act {
    template<class I = interpolator_linear<vector2f>>
    struct transform_skew_applier_t {
        com::transform* _transform;
        void operator() (vec2f_anim_kf_t const& keyframe,
                         timer::time_t offset) const {
            auto&& key = keyframe.interpolate<I>(offset);
            _transform->set_skew(key.x(), key.y());
            _transform->mark_dirty();
        }
    };
    
    template<class I = interpolator_linear<vector3f>>
    struct transform_translate_applier_t {
        com::transform* _transform;
        void operator() (vec3f_anim_kf_t const& keyframe,
                         timer::time_t offset) const {
            _transform->set_translate(keyframe.interpolate<I>(offset));
            _transform->mark_dirty();
        }
    };
    
    template<class I = interpolator_linear<vector3f>>
    struct transform_scale_applier_t {
        com::transform* _transform;
        void operator() (vec3f_anim_kf_t const& keyframe,
                         timer::time_t offset) const {
            _transform->set_scale(keyframe.interpolate<I>(offset));
            _transform->mark_dirty();
        }
    };

    template<class I = interpolator_slerp<quaternionf>>
    struct transform_rotate_applier_t {
        com::transform* _transform;
        void operator() (quaternionf_anim_kf_t const& keyframe,
                         timer::time_t offset) const {
            _transform->set_rotate(keyframe.interpolate<I>(offset));
            _transform->mark_dirty();
        }
    };

    template<class I = interpolator_linear<vector2f>>
    action_keyframe<vector2f>*
    make_skew_action(game_object* go, timer::time_t duration,
                     vec2f_anim_kf_t::const_ptr const& keyframe,
                     timer const& t = global_timer_base::instance()) {
        auto* trans = go->get_component<com::transform>();
        if (trans == nullptr)
            return nullptr;
        
        return make_action_keyframe<vector2f>(transform_skew_applier_t<I>{trans}, duration,
                                              keyframe, t);
    }
    
    template<class I = interpolator_linear<vector3f>>
    action_keyframe<vector3f>*
    make_translate_action(game_object* go, timer::time_t duration,
                          vec3f_anim_kf_t::const_ptr const& keyframe,
                          timer const& t = global_timer_base::instance()) {
        auto* trans = go->get_component<com::transform>();
        if (trans == nullptr)
            return nullptr;
        
        return make_action_keyframe<vector3f>(transform_translate_applier_t<I>{trans}, duration,
                                              keyframe, t);
    }

    template<class I = interpolator_linear<vector3f>>
    action_keyframe<vector3f>*
    make_scale_action(game_object* go, timer::time_t duration,
                      vec3f_anim_kf_t::const_ptr const& keyframe,
                      timer const& t = global_timer_base::instance()) {
        auto* trans = go->get_component<com::transform>();
        if (trans == nullptr)
            return nullptr;
        
        return make_action_keyframe<vector3f>(transform_scale_applier_t<I>{trans}, duration,
                                              keyframe, t);
    }

    template<class I = interpolator_slerp<quaternionf>>
    action_keyframe<quaternionf>*
    make_rotate_action(game_object* go, timer::time_t duration,
                       quaternionf_anim_kf_t::const_ptr const& keyframe,
                       timer const& t = global_timer_base::instance()) {
        auto* trans = go->get_component<com::transform>();
        if (trans == nullptr)
            return nullptr;
        
        return make_action_keyframe<quaternionf>(transform_rotate_applier_t<I>{trans}, duration,
                                                 keyframe, t);
    }
}
#endif