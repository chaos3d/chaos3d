#ifndef _CHAOS3D_COM_ANIM_ANIMATION_H
#define _CHAOS3D_COM_ANIM_ANIMATION_H

#include "go/component.h"
#include "action/action.h"
#include "com/anim/skeleton_transform.h"
#include <unordered_map>

namespace com {
    class transform;
    
    /// the animation component that functions as a controller
    /// the animation is updated by the action system, the controller
    /// will manage the internal state and the transition etc.
    ///
    /// this will create and manage the children game objects, i.e.
    /// the transform and sprite
    class animation : public component {
    public:
        // TODO: additive channels
        // TODO: other channels: i.e. events/uniform/skin/texture/particles
        typedef std::unordered_map<std::string, skeleton_animation_clip> clips_t;
        typedef std::unordered_map<std::string, uint32_t> names_t;
        typedef std::vector<transform*> transforms_t;
    public:
        animation(game_object*);
        
        /// create the action from the given clip name
        /// this is version 1 that each animation is separate and
        /// handled outside the controller
        action::ptr make_animation(std::string const& clip_name) const;
        
        /// apply the skin to the object structure
        bool apply_skin(std::string const&);
        
    private:
        transforms_t _transforms; // all children for the skeleton
        
        SIMPLE_CLONE(animation);
    };
}
#endif
