#ifndef _CHAOS3D_COM_ANIM_ANIMATION_H
#define _CHAOS3D_COM_ANIM_ANIMATION_H

#include "go/component.h"
#include "action/action.h"
#include "io/data_stream.h"
#include "com/anim/skeleton_transform.h"
#include "com/sprite2d/texture_atlas.h"
#include <unordered_map>
#include <map>

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

        // FIXME: better skin struct
        struct skin_piece {
            texture_atlas*  atlas;
            box2f           bound;      // sprite bound
            vector3f        translate;  // translate
            double          rotation;
        };
        typedef std::unordered_map<std::string, skin_piece> skin_t;
        typedef std::map<std::string, skin_t> skins_t;
        
        struct joint_pose {
            vector3f translate;
            vector3f scale;
            quaternionf rotate;
        };
        typedef std::vector<joint_pose> joint_poses_t;
        
    public:
        /// load animation/skeleton data from the json stream
        animation(game_object*,
                  data_stream* = nullptr, std::vector<texture_atlas*> const& = {},
                  int32_t idx = 0);
        
        /// create the action from the given clip name
        /// this is version 1 that each animation is separate and
        /// handled outside the controller
        ::action::ptr make_action(std::string const& clip_name) const;
        
        /// apply the skin to the object structure
        bool apply_skin(std::string const&);
        void add_atlas(texture_atlas*); // fill the skins from the atlas, TODO: better API
        
        /// get all the bounding children
        transforms_t const& transforms() const { return _transforms; }

        /// load the animation/skeleton data from the json stream
        bool load_from(data_stream*, std::vector<texture_atlas*> const& = {});
        
        /// get the name to index mapping
        names_t const& names() const { return _names; }
        
        /// the initial setup poses for children
        joint_poses_t const& setup_poses() const { return _setup_poses; }
        
    private:
        /// remove all data, destroy all children/game objects
        void clear();
        
        transforms_t _transforms;   // all children for the skeleton
        joint_poses_t _setup_poses; // setup poses
        names_t _names;             // joint names => joint index lookup
        names_t _slots;             // skin names => joint index lookup
        skins_t _skins;             // skins sets
        clips_t _clips;             // animation clips
        
        ATTRIBUTE(int32_t, start_index, 0); /// the starting index for the child sprites
        SIMPLE_CLONE(animation);
    };
}
#endif
