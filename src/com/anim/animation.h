#ifndef _CHAOS3D_COM_ANIM_ANIMATION_H
#define _CHAOS3D_COM_ANIM_ANIMATION_H

#include "go/component.h"
#include "action/action.h"
#include "io/data_stream.h"
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
        typedef std::unordered_map<std::string, skeleton_animation_clip::ptr> clips_t;
        typedef std::unordered_map<std::string, uint32_t> names_t;
        typedef std::vector<transform*> transforms_t;
    
    public:
        /// load animation/skeleton data from the json stream
        animation(game_object*, data_stream* = nullptr);
        
        /// create the action from the given clip name
        /// this is version 1 that each animation is separate and
        /// handled outside the controller
        ::action::ptr make_action(std::string const& clip_name) const;
        
        /// apply the skin to the object structure
        bool apply_skin(std::string const&);
        
        /// get all the bounding children
        transforms_t const& transforms() const { return _transforms; }

        /// load the animation/skeleton data from the json stream
        bool load_from(data_stream*);

    private:
        /// remove all data, destroy all children/game objects
        void clear();
        
        transforms_t _transforms;   // all children for the skeleton
        names_t _names;             // names => index lookup
        clips_t _clips;             // animation clips
        
        SIMPLE_CLONE(animation);
    };
}
#endif
