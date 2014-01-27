#ifndef _SPRITE2D_SPRITE_H
#define _SPRITE2D_SPRITE_H

#include "go/component_manager.h"
#include "com/render/renderable.h"

namespace sprite2d {
    
    // 2d sprite
    // it can be more than 1 sprite in a single component, or rather
    // the whole 2d skeleton, so it could generate more than more
    // batches especially it uses more textures
    class sprite : public com::renderable {
    public:
        typedef empty_component_mgr<sprite> manager_t;
        
    private:
        // uniform: texture, params, etc
        // raw vertices buffer
    };
    
    typedef sprite::manager_t sprite_mgr; // TODO: it will do more than this
}
#endif