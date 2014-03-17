#include "script/type/def.h"
#include "go/game_object.h"
#include "com/sprite2d/sprite.h"
#include "com/sprite2d/camera2d.h"
#include "com/sprite2d/quad_sprite.h"
#include "com/sprite2d/texture_atlas.h"
#include "script/class_type.h"
#include "script/lua_bind.h"

using namespace sprite2d;

namespace script {
    static int c3d_lua_add_layout(lua_State* L) {
        sprite_mgr& mgr = converter<sprite_mgr&>::from(L, 1, nullptr);
        sprite_mgr::vertices_t vertices;
        for (int i = 2, t = lua_gettop(L); i <= t; ++i) {
            luaL_argcheck(L, lua_istable(L, i), i, "expect a table");
            lua_rawgeti(L, -1, 1);
            lua_rawgeti(L, -2, 2);
            if (lua_isstring(L, -2)) {
                // FIXME: we assume only Float for now
                vertices.push_back({
                    lua_tostring(L, -2),
                    vertex_layout::Float,
                    (int)lua_tonumber(L, -1)
                });
            }
            lua_pop(L, 2);
        }
        lua_pushnumber(L, mgr.add_type(vertices));
        return 1;
    }
    
    void def_sprite2d() {
        class_<game_object>::type()
        .def("add_quad_sprite", LUA_BIND((&game_object::add_component<quad_sprite, int>)))
        .def("add_camera2d", LUA_BIND((&game_object::add_component<camera2d>)))
        ;
        
        class_<sprite2d::sprite>::type()
        .def("set_material", LUA_BIND_S(void (sprite::*)(sprite_material*),
                                        &sprite::set_material))
        ;
        
        class_<quad_sprite>::type()
        .derive<sprite2d::sprite>()
        .def("set_from_atlas", LUA_BIND(&quad_sprite::set_from_atlas))
        ;
        
        class_<sprite_mgr>::type()
        .def("add_material", LUA_BIND_S(sprite_material* (sprite_mgr::*)(std::string const&,
                                                                         gpu_program*,
                                                                         render_state::ptr const&,
                                                                         render_uniform::ptr const&),
                                        &sprite_mgr::add_material))
        .def("vertex_layout", LUA_BIND(&sprite_mgr::vertex_layout))
        .def("add_layout", c3d_lua_add_layout)
        ;
        
        
        class_<camera2d>::type()
        ;
#if 0
        script::class_<texture_atlas>::type()
        ;
#endif
        
   }
}

