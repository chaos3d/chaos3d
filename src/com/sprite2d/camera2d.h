#ifndef _SPRITE2D_CAMERA2D_H
#define _SPRITE2D_CAMERA2D_H

#include "com/render/camera.h"
#include <vector>

namespace sprite2d {
    class sprite;
    
    // 2D atlas batched sprite renderer
    class camera2d : public com::camera {
    public:
        camera2d(game_object*, render_target* = nullptr, int priority = 0);
        
    protected:
        camera2d& operator=(camera2d const& rhs);

        // TODO: the sprite may have a different config for AABB to do culling
        // instead of using collider2d
        virtual void collect(std::vector<game_object*> const&) override;
        virtual void do_render(com::camera_mgr const&) override;
        
    private:
        // local cache for sorting sprites, not persistent
        std::vector<sprite*> _sorted_sprites;
        
        SIMPLE_CLONE(camera2d);
        COMPONENT_FROM_LOADER(camera2d, camera2d);
    };
}
#endif