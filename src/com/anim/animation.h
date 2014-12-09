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
    class animation_mgr;
    
    /// the animation component that functions as a controller
    /// the animation is updated by the action system, the controller
    /// will manage the internal state and the transition etc.
    ///
    /// this will create and manage the children game objects, i.e.
    /// the transform and sprite
    class animation : public component {
    public:
        typedef animation_mgr manager_t;
        
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
        
        struct slot {
            uint32_t    joint_index;    // joint index in joint pose array
            int32_t     sprite_index;   // sprite drawing index
            std::string piece_name;     // skin piece name
            
            slot(uint32_t idx, int32_t sid, char const* name)
            : joint_index(idx), sprite_index(sid), piece_name(name)
            {}
        };
        typedef std::unordered_map<std::string, slot> slots_t;
        
    public:
        /// load animation/skeleton data from the json stream
        animation(game_object*,
                  data_stream* = nullptr, std::vector<texture_atlas*> const& = {},
                  int32_t idx = 0);

        void play(std::string const& name);

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
        /// load skin, template to remove dependencies
        template<typename C>
        void load_skin(std::string const&, C const&);
        
        /// remove all data, destroy all children/game objects
        void clear();
        
        virtual void destroy() override;
        animation& operator=(animation const& rhs);
        
        transforms_t _transforms;   // all children for the skeleton
        joint_poses_t _setup_poses; // setup poses
        names_t _names;             // joint names => joint index lookup
        slots_t _slots;             // skin names => slot config
        skins_t _skins;             // skins sets
        clips_t _clips;             // animation clips
        
        // flag for the manager to defer removing until the update
        bool _mark_for_remove = false;

        // action root
        action::ptr _action;
        
        /// the starting index for the child sprites
        ATTRIBUTE(int32_t, start_index, 0);
        SIMPLE_CLONE(animation);

        friend class animation_mgr;
    };

    /// manage animations and update them
    class animation_mgr : public component_manager_base<animation_mgr> {
    public:
        typedef std::false_type component_fixed_t;
        typedef std::unique_ptr<animation> animation_ptr;
        typedef std::forward_list<animation_ptr> animations_t;
        
    protected:
        // add the animation to be managed
        void add_animation(animation*);
        
        virtual void pre_update(goes_t const&) override;
        virtual void update(goes_t const&) override {};

    private:
        animations_t _animations;

        friend class animation;
    };
}
#endif
