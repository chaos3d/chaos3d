#ifndef _CHAOS3D_COM_ANIM_SKELETON_TRANSFORM_H
#define _CHAOS3D_COM_ANIM_SKELETON_TRANSFORM_H

#include "sg/transform.h"
#include "action/action_keyframe.h"
#include "common/base_types.h"
#include "common/timer.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace com {
    class transform;
    
    // TODO: use metal for gpu computing?
    // keyframe/transform both do matrix/vector computations
    // should collect them all into metal
    
    /// skeleton data for one animation clip
    /// the animation clip consists of several channels, each of
    /// which represents one bone/joint's keyframe-based animation
    /// data. each joint represents its local matrix.
    /// this calculates local matrix, where the transform component
    /// handles the parent hierarchy.
    class skeleton_animation_clip : public std::enable_shared_from_this<skeleton_animation_clip> {
    public:
        typedef std::shared_ptr<skeleton_animation_clip> ptr;
        typedef timer::time_t time_t;
        
        struct joint_pose {
            // TODO: interpolation
            vector3f translate;
            vector3f scale;
            vector2f skew;
            quaternionf rotation;
        };
        
        typedef animation_keyframe<joint_pose> joint_channel;
        
        typedef std::vector<joint_pose> joint_poses_t;
        typedef std::vector<transform*> transforms_t;
        
        // joint index, and its channel data
        typedef std::forward_list<std::tuple<uint32_t, joint_channel>> joint_channels_t;
        
    public:
        void apply(time_t offset, transforms_t const&);
        
    private:
        joint_channels_t _channels;
    };
    
}

#endif
