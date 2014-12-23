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

class data_stream;

namespace com {
    
    // TODO: use metal for gpu computing?
    // keyframe/transform both do matrix/vector computations
    // should collect them all into metal
    
    /// skeleton data for one animation clip
    /// the animation clip consists of several channels, each of
    /// which represents one bone/joint's keyframe-based animation
    /// data. each joint represents its local matrix.
    /// this calculates local matrix, where the transform component
    /// handles the parent hierarchy.
    class animation_clip : public std::enable_shared_from_this<animation_clip> {
    public:
        typedef std::shared_ptr<animation_clip> ptr;
        typedef timer::time_t time_t;
        
        // animation data channel
        typedef animation_keyframe<vector3f> translate_channel_t;
        typedef animation_keyframe<vector3f> scale_channel_t;
        typedef animation_keyframe<quaternionf> rotate_channel_t;
        
        struct joint_channel {
            // this shall belong to the clip, no external reference
            // is necessary
            typedef std::unique_ptr<joint_channel> ptr;
            
            translate_channel_t::ptr    translate;
            scale_channel_t::ptr        scale;
            rotate_channel_t::ptr       rotate;
            // TODO: slots/texture/event channels
        };
        
        typedef std::vector<joint_channel*> channels_t;
        typedef std::unordered_map<std::string, uint32_t> names_t;
        typedef std::vector<joint_channel::ptr> joint_channels_t;
        
    public:
        /// generic loader without any references
        template<typename ...Args> animation_clip(Args const&...);

        template<typename ...Args>
        static animation_clip::ptr load_from(Args&&... args) {
            return animation_clip::ptr(new animation_clip(std::forward<Args>(args)...));
        }
        
        /// initialize data from a data stream (JSON)
        animation_clip(data_stream*);
        
        /// giving the names, replace the channel data based on the corresponding
        /// channel indices.
        /// @return number of channels is added
        uint32_t get_channels(names_t const&, channels_t&) const;
        
    private:
        void load_from(data_stream*); // load and initialize data
        
        /// the channel/joint name to index mapping
        names_t _names;
        
        /// channel data for each joint
        joint_channels_t _channels;
    };
    
}

#endif
