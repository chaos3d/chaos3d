#ifndef _SPRITE2D_QUAD_SPRITE_H
#define _SPRITE2D_QUAD_SPRITE_H

#include "com/sprite2d/sprite.h"
#include "go/game_object.h"
#include "Eigen/Geometry"

namespace sprite2d {
    
    // the quad sprite
    class quad_sprite : public sprite {
    public:
        typedef Eigen::AlignedBox2f box2f;
        
    public:
        quad_sprite(game_object*, int type);
        
        ATTRIBUTE(box2f, frame);
        ATTRIBUTE(box2f, bound);
        
        void set_dirty() const { parent()->set_flag(sprite_mgr::flag_offset()); }
        
    protected:
        virtual quad_sprite* clone(game_object*) const override;
        
    private:
        virtual void fill_buffer(void* buffer, size_t stride, com::transform const&) const override;
        
    };
}

#endif