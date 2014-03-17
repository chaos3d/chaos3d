#include "script/type/def.h"
#include "go/game_object.h"
#include "com/sprite2d/sprite.h"
#include "com/sprite2d/quad_sprite.h"
#include "com/sprite2d/texture_atlas.h"
#include "script/class_type.h"
#include "script/lua_bind.h"

using namespace sprite2d;

namespace script {
    void def_sprite2d() {
        script::class_<game_object>::type()
        .def("add_quad_sprite", LUA_BIND((&game_object::add_component<sprite2d::quad_sprite, int>)))
        ;
        
        script::class_<sprite2d::sprite>::type()
        .def("set_material", LUA_BIND_S(void (sprite::*)(sprite_material*),
                                        &sprite::set_material))
        ;
        
        script::class_<quad_sprite>::type()
        .derive<sprite2d::sprite>()
        .def("set_from_atlas", LUA_BIND(&quad_sprite::set_from_atlas))
        ;
        
        script::class_<sprite_mgr>::type()
        .def("add_material", LUA_BIND_S(sprite_material* (sprite_mgr::*)(std::string const&,
                                                                         gpu_program*,
                                                                         render_state::ptr const&,
                                                                         render_uniform::ptr const&),
                                        &sprite_mgr::add_material))
        ;
        
#if 0
        script::class_<texture_atlas>::type()
        ;
#endif
        
   }
}

