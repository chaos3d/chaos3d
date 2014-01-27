#ifndef _SPRITE2D_CAMERA2D_H
#define _SPRITE2D_CAMERA2D_H

#include "com/render/camera.h"

namespace sprite2d {
    // 2D atlas batched sprite renderer
    class camera2d : public com::camera {
    public:
        camera2d(game_object*, int priority = 0);
        
    protected:
        virtual void collect(std::vector<game_object*> const&) override;
        virtual void do_render(com::render_component_mgr const&) override;
        
        SIMPLE_CLONE(camera2d);
    };
}
#endif