#ifndef _SPRITE2D_SPRITE_H
#define _SPRITE2D_SPRITE_H

#include "go/component_manager.h"
#include "com/render/renderable.h"

namespace sprite2d {
    
    class sprite : public com::renderable {
    public:
        typedef empty_component_mgr<sprite> manager_t;
    };
    
    typedef sprite::manager_t sprite_mgr;
}
#endif