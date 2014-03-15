#include "script/type/def.h"
#include "script/class_type.h"
#include "script/lua_bind.h"

namespace script {
    void def_render_device() {
        script::class_<render_device>::type()
        .def("create_shader", LUA_BIND(&render_device::create_shader))
        ;
        
        script::class_<gpu_shader>::type()
        .def("compile", LUA_BIND_S(void (gpu_shader::*)(char const*), &gpu_shader::compile))
        .def("compile_multi", LUA_BIND_S(void (gpu_shader::*)(std::vector<char const*> const&),
                                         &gpu_shader::compile))
        ;
        
    }
}

