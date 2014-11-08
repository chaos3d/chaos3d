#ifndef _SPRITE2D_QUAD_SPRITE_H
#define _SPRITE2D_QUAD_SPRITE_H

#include "com/sprite2d/sprite.h"
#include <Eigen/Geometry>

class texture_atlas;

namespace sprite2d {
    
    // the quad sprite
    class quad_sprite : public sprite {
    public:
        typedef Eigen::AlignedBox2f box2f;
        typedef Eigen::Vector2f vector2f;
        typedef Eigen::Vector4f vector4f;
        
        struct animated_frame_key {
            enum { FRAME = 1, BOUND = 2, MAT = 4 };
            box2f frame;    // texture uv
            box2f bound;    // upper/lower bound
            sprite_material* mat = nullptr;
            uint8_t mask = 1;
            
            animated_frame_key(box2f const& frame_,
                               sprite_material* mat_ = nullptr)
            : frame(frame_), mat(mat_),
            mask(FRAME | (mat_ != nullptr ? MAT : 0))
            {}
            
            animated_frame_key() = default;
        };
        
        typedef box2f animated_bound_key;
        typedef float animated_alpha;
        
    public:
        quad_sprite(game_object*, int type);
        quad_sprite(game_object*, texture_atlas const&,
                    std::string const& name,
                    std::string const& mat = "basic");
        
        void set_frame_key(animated_frame_key const& key) {
            if (key.mask & animated_frame_key::FRAME)
                set_frame(key.frame);
            if (key.mask & animated_frame_key::BOUND)
                set_bound(key.bound);
            if (key.mask & animated_frame_key::MAT && key.mat)
                set_material(key.mat);
        }

        // set the material and its bound/frame at once
        // the bound is calculated based on the size of texture
        // and the pivot
        quad_sprite& set_from_material(sprite_material* mat,
                                       box2f const& frame,
                                       vector2f const& pivot = {0.f, 0.f});
        
        quad_sprite& set_from_atlas(texture_atlas const&,
                                    std::string const& name,
                                    std::string const& mat = "basic");
    protected:
        virtual quad_sprite* clone(game_object*) const override;
        
    private:
        virtual void fill_buffer(vertex_layout::locked_buffer const& buffer,
                                 com::transform const&) const override;
        virtual void fill_indices(uint16_t) override;
        
        ATTRIBUTE(box2f, frame, box2f()); // texture uv
        ATTRIBUTE(box2f, bound, box2f()); // lower and upper bound
        ATTRIBUTE(float, alpha, 1.f); // alpha
        COMPONENT_FROM_LOADER(quad_sprite, quad);
    };
}

#endif