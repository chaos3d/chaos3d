#ifndef _SPRITE2D_CAMERA2D_H
#define _SPRITE2D_CAMERA2D_H

#include "com/render/camera.h"

namespace sprite2d {
    // 2D atlas batched sprite renderer
    class camera2d : public com::camera {
    public:
        camera2d(game_object*, int priority = 0);
        
        // the z-/depth value for the plane where a pixel
        // is equal to '1' based on the current target
        // and the perspective matrix
        float distant_for_perfect_pixel() const {
            return target()->size().y() * .5f * proj_matrix()(1,1);
        }
        
        // move the camera to where it is pixel perfect
        // on the plane z = 0
        // NB: if transform doesn't exist, one will be created
        camera2d& move_for_perfect_pixel();
        
    protected:
        camera2d& operator=(camera2d const& rhs);

        // TODO: the sprite may have a different config for AABB to do culling
        // instead of using collider2d
        virtual void collect(std::vector<game_object*> const&) override;
        virtual void do_render(com::render_component_mgr const&) override;
        
        SIMPLE_CLONE(camera2d);
        COMPONENT_FROM_LOADER(camera2d, camera2d);
    };
}
#endif