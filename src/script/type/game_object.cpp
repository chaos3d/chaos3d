#include "script/type/def.h"
#include "go/game_object.h"
#include "sg/transform.h"
#include "script/class_type.h"
#include "script/lua_bind.h"

namespace script {
    void def_render_device() {
        script::class_<game_object>::type()
        .def("by_tag", LUA_BIND(&game_object::find_by_tag))
        .def("set_tag", LUA_BIND(&game_object::set_tag<char const*>))
        .def("add_transform", LUA_BIND(&game_object::add_component<com::transform>))
        ;
        
        script::class_<com::transform>::type()
        .def("set_translate", LUA_BIND((&com::transform::set_translate<float, float, float>)))
        .def("set_rotate", LUA_BIND((&com::transform::set_rotate_by_axis)))
        .def("set_scale", LUA_BIND((&com::transform::set_scale<float, float, float>)))
        ;
    }
}