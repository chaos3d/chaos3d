#ifndef _SPRITE2D_QUAD_SPRITE_H
#define _SPRITE2D_QUAD_SPRITE_H

#include "com/sprite2d/sprite.h"
#include "Eigen/Geometry"

namespace sprite2d {
    class quad_sprite : public sprite {
    public:
        typedef Eigen::AlignedBox2f box2f;
        
    public:
        quad_sprite(game_object*, int type);
        
        ATTRIBUTE(box2f, frame);
        ATTRIBUTE(box2f, bound);
        
    private:
        // fill the vertices into the buffer
        // one shouldn't use more than it requested
        virtual void fill_buffer(void* buffer, size_t stride, com::transform const&) const override;
        
    };
}

#endif