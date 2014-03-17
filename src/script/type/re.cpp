#include "script/type/def.h"
#include "script/class_type.h"
#include "script/lua_bind.h"
#include "script/state.h"
#include "script/converter_ptr.h"
#include "re/render_device.h"
#include "com/render/camera.h"

namespace script {
    static int c3d_lua_state_set(lua_State* L) {
        render_state& state = converter<render_state&>::from(L, 1, nullptr);
        luaL_argcheck(L, lua_istable(L, 2), 2, "expect a table");
        
        lua_pushnil(L);
#define VALIDATE(v, min, max, field) \
        if (v < min && v >= max) { \
            luaL_argerror(L, 2, #field " not in the valid range"); \
        }
        while (lua_next(L, 2)) {
            if (!lua_isstring(L, -2)) {
                lua_pop(L, 1);
                continue;
            }
            
            char const* key = lua_tostring(L, -2);
            if (strcmp(key, "src_blend") == 0) {
                uint8_t src = converter<uint8_t>::from(L, -1, nullptr);
                VALIDATE(src, render_state::BlendNone, render_state::BlendMax, "src_blend");
                state.set_src_blend(src);
            } else if (strcmp(key, "dst_blend") == 0) {
                uint8_t dst = converter<uint8_t>::from(L, -1, nullptr);
                VALIDATE(dst, render_state::BlendNone, render_state::BlendMax, "dst_blend");
                state.set_dst_blend(dst);
            } else if (strcmp(key, "src_alpha") == 0) {
                uint8_t src = converter<uint8_t>::from(L, -1, nullptr);
                VALIDATE(src, render_state::BlendNone, render_state::BlendMax, "src_alpha");
                state.set_dst_alpha_blend(src);
            } else if (strcmp(key, "dst_alpha") == 0) {
                uint8_t dst = converter<uint8_t>::from(L, -1, nullptr);
                VALIDATE(dst, render_state::BlendNone, render_state::BlendMax, "dst_alpha");
                state.set_dst_alpha_blend(dst);
            }
            lua_pop(L, 1); // pop the value
        }
#undef VALIDATE
        lua_settop(L, 1);
        return 1;
    }
    
    static int c3d_lua_create_uniform(lua_State* L) {
        converter<render_uniform::ptr>::to(L, std::make_shared<render_uniform>());
        return 1;
    }
    
    static int c3d_lua_create_state(lua_State* L) {
        converter<render_state::ptr>::to(L, std::make_shared<render_state>());
        return 1;
    }
    
    void def_render_device(state* st, std::string const& scope) {
        st->import((scope + ".blend").c_str())
        .import<uint8_t>(LUA_ENUM(render_state, BlendNone))
        .import<uint8_t>(LUA_ENUM(render_state, BlendOne))
        .import<uint8_t>(LUA_ENUM(render_state, BlendZero))
        .import<uint8_t>(LUA_ENUM(render_state, BlendDstColor))
        .import<uint8_t>(LUA_ENUM(render_state, BlendSrcColor))
        .import<uint8_t>(LUA_ENUM(render_state, BlendOneMinusDstColor))
        .import<uint8_t>(LUA_ENUM(render_state, BlendOneMinusSrcColor))
        .import<uint8_t>(LUA_ENUM(render_state, BlendDstAlpha))
        .import<uint8_t>(LUA_ENUM(render_state, BlendSrcAlpha))
        .import<uint8_t>(LUA_ENUM(render_state, BlendOneMinusDstAlpha))
        .import<uint8_t>(LUA_ENUM(render_state, BlendOneMinusSrcAlpha))
        ;
        
        st->import((scope + ".shader").c_str())
        .import<uint8_t>(LUA_ENUM(gpu_shader, Vertex))
        .import<uint8_t>(LUA_ENUM(gpu_shader, Fragment))
        ;

        class_<render_device>::type()
        .def("create_shader", LUA_BIND(&render_device::create_shader))
        .def("create_program", LUA_BIND(&render_device::create_program))
        .def("create_uniform", c3d_lua_create_uniform)
        .def("create_state", c3d_lua_create_state)
        ;
        
        class_<render_uniform>::type()
        .def("set_float", LUA_BIND_S(void (render_uniform::*)(std::string const&, float),
                                     &render_uniform::set_vector))
        .def("set_vec2", LUA_BIND_S(void (render_uniform::*)(std::string const&,
                                                             float, float),
                                    &render_uniform::set_vector))
        .def("set_vec3", LUA_BIND_S(void (render_uniform::*)(std::string const&,
                                                             float, float, float),
                                    &render_uniform::set_vector))
        .def("set_vec4", LUA_BIND_S(void (render_uniform::*)(std::string const&,
                                                             float, float, float, float),
                                    &render_uniform::set_vector))
        .def("set_texture", LUA_BIND(&render_uniform::set_texture))
        ;
        
        class_<render_state>::type()
        .def("set", c3d_lua_state_set)
        ;
        
        class_<gpu_shader>::type()
        .def("compile", LUA_BIND_S(void (gpu_shader::*)(char const*), &gpu_shader::compile))
        .def("compile_multi", LUA_BIND_S(void (gpu_shader::*)(std::vector<char const*> const&),
                                         &gpu_shader::compile))
        ;
        
        class_<gpu_program>::type()
        .def("link", LUA_BIND(&gpu_program::link))
        ;
        
        class_<com::camera>::type()
        .def("set_perspective", LUA_BIND_S(com::camera& (com::camera::*)(float, float, float, float),
                                           &com::camera::set_perspective))
        .def("set_target", LUA_BIND(&com::camera::set_target))
        .def("distance", LUA_BIND(&com::camera::distant_for_perfect_pixel))
        ;
    }
}

