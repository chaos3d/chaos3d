#ifndef _SPRITE2D_QUAD_SPRITE_H
#define _SPRITE2D_QUAD_SPRITE_H

#include "com/sprite2d/sprite.h"
#include <Eigen/Geometry>

namespace sprite2d {
    
    // the quad sprite
    class quad_sprite : public sprite {
    public:
        typedef Eigen::AlignedBox2f box2f;
        typedef Eigen::Vector4f vector4f;
        
        struct animated_frame_key {
            box2f frame;
            sprite_material* mat = nullptr;
        };
        
        typedef box2f animated_bound_key;
        typedef float animated_alpha;
        
    public:
        quad_sprite(game_object*, int type);
        
        ATTRIBUTE(box2f, frame); // texture uv
        ATTRIBUTE(box2f, bound); // position and size
        ATTRIBUTE(float, alpha); // alpha
        
        void set_frame_key(animated_frame_key const& key) {
            set_frame(key.frame);
            if (key.mat)
                set_material(key.mat);
        }

    protected:
        virtual quad_sprite* clone(game_object*) const override;
        
    private:
        virtual void fill_buffer(vertex_layout::locked_buffer const& buffer,
                                 com::transform const&) const override;
        virtual void fill_indices(uint16_t) override;
        
    };
}

#endif