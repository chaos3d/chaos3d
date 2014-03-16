#include "script/type/def.h"
#include "script/class_type.h"
#include "script/lua_bind.h"
#include "script/converter_ptr.h"
#include "re/render_device.h"
#include "com/render/camera.h"

namespace script {
    static int c3d_lua_create_uniform(lua_State* L) {
        converter<render_uniform>::to(L, std::make_shared<render_uniform>());
        return 1;
    }
    
    void def_render_device() {
        script::class_<render_device>::type()
        .def("create_shader", LUA_BIND(&render_device::create_shader))
        .def("create_uniform", c3d_lua_create_uniform)
        ;
        
        script::class_<render_uniform>::type()
        .def("set_float", LUA_BIND_S(void (render_uniform::*)(std::string const&, float),
                                     &render_uniform::set_vector))
        .def("set_vec2", LUA_BIND_S(void (render_uniform::*)(std::string const&, float, float),
                                    &render_uniform::set_vector))
        .def("set_vec3", LUA_BIND_S(void (render_uniform::*)(std::string const&, float, float,
                                                             float),
                                    &render_uniform::set_vector))
        .def("set_vec4", LUA_BIND_S(void (render_uniform::*)(std::string const&, float, float,
                                                             float, float),
                                    &render_uniform::set_vector))
        .def("set_texture", LUA_BIND(&render_uniform::set_texture))
        ;
        
        script::class_<gpu_shader>::type()
        .def("compile", LUA_BIND_S(void (gpu_shader::*)(char const*), &gpu_shader::compile))
        .def("compile_multi", LUA_BIND_S(void (gpu_shader::*)(std::vector<char const*> const&),
                                         &gpu_shader::compile))
        ;
        
        script::class_<com::camera>::type()
        .def("set_perspective", LUA_BIND_S(com::camera& (com::camera::*)(float, float, float, float),
                                           &com::camera::set_perspective))
        ;
    }
}

