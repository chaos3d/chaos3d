#include "script/type/def.h"
#include "go/game_object.h"
#include "com/sprite2d/sprite.h"
#include "com/sprite2d/quad_sprite.h"
#include "com/sprite2d/texture_atlas.h"
#include "script/class_type.h"
#include "script/lua_bind.h"

namespace script {
    void def_sprite2d() {
        script::class_<game_object>::type()
        .def("add_quad_sprite", LUA_BIND((&game_object::add_component<sprite2d::quad_sprite, int>)))
        ;
        
        script::class_<sprite2d::quad_sprite>::type()
        .def("set_from_atlas", LUA_BIND(&sprite2d::quad_sprite::set_from_atlas))
        ;
        
#if 0
        script::class_<sprite2d::sprite_mgr>::type()
        ;
        
        script::class_<texture_atlas>::type()
        ;
#endif
        
   }
}

