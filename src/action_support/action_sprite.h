#ifndef _CHAOS3D_ACTION_SPRITE_H
#define _CHAOS3D_ACTION_SPRITE_H

#include "action/action_keyframe.h"
#include "com/sprite2d/quad_sprite.h"

namespace act {
    typedef sprite2d::quad_sprite::animated_frame_key sprite_key_t;
    typedef animation_keyframe<sprite_key_t> sprite_anim_kf_t;
    typedef sprite_anim_kf_t::key_frames_t sprite_keyframes_t;
    
    struct quad_sprite_applier_t {
        sprite2d::quad_sprite* _sprite;
        void operator() (sprite_anim_kf_t const& keyframes,
                         timer::time_t offset) const {
            _sprite->set_frame_key(keyframes.interpolate<void>(offset));
            _sprite->mark_dirty();
        }
    };
    
    typedef sprite2d::quad_sprite::animated_atlas_key atlas_key_t;
    typedef animation_keyframe<atlas_key_t> atlas_anim_kf_t;
    typedef atlas_anim_kf_t::key_frames_t atlas_keyframes_t;
    
    struct quad_atlas_applier_t {
        sprite2d::quad_sprite* _sprite;
        atlas_anim_kf_t::key_frame_t::key_t const* current = nullptr;
        
        quad_atlas_applier_t(sprite2d::quad_sprite* sprite)
        : _sprite(sprite) {}
        
        void operator() (atlas_anim_kf_t const& keyframes,
                         timer::time_t offset) {
            auto& key = keyframes.interpolate<void>(offset);
            if (&key == current)
                return;
            
            current = &key;
            _sprite->set_from_atlas(*key.atlas, key.name, key.mat);
            _sprite->mark_dirty();
        }
    };
    
    action::ptr make_sprite_action(game_object* go, timer::time_t duration,
                                   sprite_anim_kf_t::const_ptr const& keyframe,
                                   timer const& t = global_timer_base::instance());

    action::ptr make_atlas_action(game_object* go, timer::time_t duration,
                                  atlas_anim_kf_t::const_ptr const& keyframe,
                                  timer const& t = global_timer_base::instance());

}

#endif
