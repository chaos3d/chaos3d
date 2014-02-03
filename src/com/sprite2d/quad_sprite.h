#ifndef _SPRITE2D_QUAD_SPRITE_H
#define _SPRITE2D_QUAD_SPRITE_H

#include "com/sprite2d/sprite.h"
#include "Eigen/Geometry"

namespace sprite2d {
    
    // the quad sprite
    class quad_sprite : public sprite {
    public:
        typedef Eigen::AlignedBox2f box2f;
        
    public:
        quad_sprite(game_object*, int type);
        
        ATTRIBUTE(box2f, frame); // texture uv
        ATTRIBUTE(box2f, bound); // position and size
        
    protected:
        virtual quad_sprite* clone(game_object*) const override;
        
    private:
        virtual void fill_buffer(void* buffer, size_t stride, com::transform const&) const override;
        virtual void fill_indices(uint16_t) override;
        
    };
}

#endif