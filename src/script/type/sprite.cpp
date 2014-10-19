#include "script/type/def.h"
#include "script/class_type.h"
#include "script/lua_bind.h"
#include "script/state.h"

#include "go/game_object.h"
#include "com/sprite2d/sprite.h"
#include "com/sprite2d/camera2d.h"
#include "com/sprite2d/quad_sprite.h"
#include "com/sprite2d/texture_atlas.h"

#include "com/scene2d/world_box2d.h"
#include "com/scene2d/shape_desc.h"

#include "asset/asset_manager.h"
#include "loader/json/json_loader.h"

using namespace sprite2d;
using namespace scene2d;

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
    
    static int c3d_lua_set_gravity(lua_State* L) {
        world2d_mgr& mgr = converter<world2d_mgr&>::from(L, 1, nullptr);
        float x = lua_tonumber(L, 2);
        float y = lua_tonumber(L, 3);
        mgr.set_gravity(vector2f(x, y));
        lua_settop(L, 1);
        return 1;
    }
    
    static int c3d_lua_world_query(lua_State* L) {
        world2d_mgr& mgr = converter<world2d_mgr&>::from(L, 1, nullptr);
        luaL_argcheck(L, lua_isfunction(L, 4), 4, "expect a function");
        float x = lua_tonumber(L, 2);
        float y = lua_tonumber(L, 3);
        mgr.query([=] (collider2d *c) {
            lua_pushvalue(L, 4);
            converter<collider2d*>::to(L, c);
            if (lua_pcall(L, 1, 1, 0) != 0) {
                // TODO: error
                printf("error: %s", lua_tostring(L, -1));
                return false; // stop querying
            } else {
                bool is_done = lua_toboolean(L, -1);
                lua_pop(L, 1);
                return !is_done;
            }
        }, vector2f(x, y));
        lua_settop(L, 1);
        return 1;
    }
    
    static int c3d_lua_cast_from_screen(lua_State* L) {
        auto& camera = converter<camera2d&>::from(L, 1, nullptr);
        
        auto ray = camera.cast_from_screen({lua_tonumber(L, 2), lua_tonumber(L, 3)});
        float t = -ray.p.z() / ray.d.z();
        auto cross = ray.p + ray.d * t;
        lua_pushnumber(L, cross[0]);
        lua_pushnumber(L, cross[1]);
        return 2;
    }
    
    static int c3d_lua_atlas_load(lua_State* L) {
        data_stream& ds = converter<data_stream&>::from(L, 1, nullptr);
        asset_manager& am = converter<asset_manager&>::from(L, 2, nullptr);
        
        auto atlas_ptr = texture_atlas::load_from<json_loader, asset_manager&, texture_atlas::TexturePacker&&>
        (json_document(&ds).as_json_loader(), am, texture_atlas::TexturePacker());

        converter<decltype(atlas_ptr)>::to(L, std::move(atlas_ptr));
        return 1;
    }
    
    static int c3d_lua_create_shape(lua_State* L) {
        typedef std::unique_ptr<shape> ptr;
        if (lua_gettop(L) == 0) {
            converter<ptr>::to(L, ptr(new shape()));
            return 1;
        }
                               
        shape* sp = nullptr;
        luaL_checktype(L, 1, LUA_TTABLE);
        
        lua_getfield(L, 1, "type");
        char const* type = lua_isstring(L, -1) ? lua_tostring(L, -1) : nullptr;
        if (type == nullptr) {
            sp = new shape();
        } else if (strcmp(type, "box")) {
            lua_getfield(L, 1, "x");
            lua_getfield(L, 1, "y");
            lua_getfield(L, 1, "w");
            lua_getfield(L, 1, "h");
            lua_getfield(L, 1, "angle");
            sp = new box(lua_tonumber(L, -5), lua_tonumber(L, -4), lua_tonumber(L, -3),
                         lua_tonumber(L, -2), lua_tonumber(L, -1));
            lua_pop(L, 5);
        } else if (strcmp(type, "circle")) {
            lua_getfield(L, 1, "x");
            lua_getfield(L, 1, "y");
            lua_getfield(L, 1, "r");
            
            sp = new circle(lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1));
            lua_pop(L, 3);
        } else {
            luaL_argerror(L, 1, "type not valid");
            assert(sp != nullptr);
        }
        
        int t = lua_gettop(L);
        lua_getfield(L, 1, "rest");
        lua_getfield(L, 1, "friction");
        lua_getfield(L, 1, "density");
        lua_getfield(L, 1, "group");
        lua_getfield(L, 1, "mask");
        lua_getfield(L, 1, "category");
        lua_getfield(L, 1, "collidable");
#define ASSIGN(top, v) (lua_isnoneornil(L, t + top) ? (v) : lua_tonumber(L, t + top))
        sp->restitution() = ASSIGN(1, 0.f);
        sp->friction() = ASSIGN(2, 0.2f);
        sp->density() = ASSIGN(3, 1.f);
        sp->collision_group() = (int16_t)ASSIGN(4, 0);
        sp->collision_mask() = (uint16_t)ASSIGN(5, 0xFFFF);
        sp->collision_category() = (uint16_t)ASSIGN(6, 0x1);
        sp->is_collidable() = lua_isnoneornil(L, t + 7) ? false : lua_toboolean(L, t + 7);
#undef ASSIGN
        converter<ptr>::to(L, ptr(sp));
        return 1;
    }

    static int c3d_lua_create_mass(lua_State* L) {
        typedef std::unique_ptr<mass> ptr;
        mass* ms = new mass();
        ms->x = lua_tonumber(L, 1);
        ms->y = lua_tonumber(L, 2);
        ms->rotate = lua_tonumber(L, 3);
        ms->weight = lua_tonumber(L, 4);
        converter<ptr>::to(L, ptr(ms));
        return 1;
    }
    
    void def_sprite2d(state* st, std::string const& scope) {
        st->import((scope + ".collider").c_str())
        .import<uint8_t>(LUA_ENUM(collider2d, dynamic))
        .import<uint8_t>(LUA_ENUM(collider2d, kinametic))
        .import<uint8_t>("static", collider2d::static_)
        .import<uint8_t>("normal", collider2d::type_normal)
        .import<uint8_t>("character", collider2d::type_character)
        .import<uint8_t>("bullet", collider2d::type_bullet)
        .def("shape", c3d_lua_create_shape)
        .def("mass", c3d_lua_create_mass)
        ;
        
        st->import((scope + ".evt").c_str())
        .import("contact_began", &typeid(contact_began_event))
        .import("contact_ended", &typeid(contact_ended_event))
        .import_type<contact_began_event>()
        .import_type<contact_ended_event>()
        ;
        
        st->import(scope.c_str())
        .def("load_atlas", c3d_lua_atlas_load)
        ;
        
        class_<game_object>::type()
        .def("add_quad_sprite", LUA_BIND((&game_object::add_component<quad_sprite, int>)))
        .def("add_camera2d", LUA_BIND((&game_object::add_component<camera2d, render_target*, int>)))
        .def("add_collider2d", LUA_BIND((&game_object::add_component<collider2d, int, int>)))
        .def("get_camera2d", LUA_BIND((&game_object::get_component<camera2d>)))
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
        .def("get_material", LUA_BIND(&sprite_mgr::find_first_material))
        .def("vertex_layout", LUA_BIND(&sprite_mgr::vertex_layout))
        .def("add_layout", c3d_lua_add_layout)
        ;
        
        
        class_<camera2d>::type()
        .derive<com::camera>()
        .def("cast_from_screen", c3d_lua_cast_from_screen)
        ;
        
        class_<collider2d>::type()
        .def("reset_shapes", LUA_BIND_S(collider2d& (collider2d::*)(std::vector<shape*> const&,
                                                                    mass const&),
                                        &collider2d::reset_shapes))
        .def("from_quad_sprite", LUA_BIND(&collider2d::reset_from<quad_sprite>))
        ;
        
        class_<world2d_mgr>::type()
        .derive<event_dispatcher>()
        .def("set_gravity", c3d_lua_set_gravity)
        .def("query", c3d_lua_world_query)
        .def("set_step", LUA_BIND_S(world2d_mgr& (world2d_mgr::*)(float&&), &world2d_mgr::set_step))
        ;
        
        class_<global_asset_mgr>::type()
        .derive<asset_manager>()
        ;
#if 0
        script::class_<texture_atlas>::type()
        ;
#endif
        
   }
}

