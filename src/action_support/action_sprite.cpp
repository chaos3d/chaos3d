#include "action_support/action_sprite.h"

namespace act {
    
    action_keyframe<sprite_key_t, quad_sprite_applier_t>*
    make_sprite_action(game_object* go, timer::time_t duration,
                       sprite_anim_kf_t::const_ptr const& keyframe,
                       timer const& t) {
        auto* sprite = go->get_component<sprite2d::quad_sprite>();
        if (sprite == nullptr)
            return nullptr;
        
        return make_action_keyframe<sprite_key_t>(quad_sprite_applier_t{sprite}, duration,
                                                  keyframe, t);
    }
    
    action_keyframe<atlas_key_t, quad_atlas_applier_t>*
    make_atlas_action(game_object* go, timer::time_t duration,
                      atlas_anim_kf_t::const_ptr const& keyframe,
                      timer const& t) {
        
        auto* sprite = go->get_component<sprite2d::quad_sprite>();
        if (sprite == nullptr)
            return nullptr;
        
        return make_action_keyframe<atlas_key_t>(quad_atlas_applier_t{sprite}, duration,
                                                 keyframe, t);
    }

}