#include "go/game_object.h"
#include "sg/transform.h"
#include "script/state.h"
#include "script/class_type.h"
#include "script/lua_bind.h"

namespace script {
    void def_game_object(state* st, std::string const& scope) {
        st->import((scope + ".go").c_str())
        .def("new", LUA_BIND(&game_object::make))
        .import("root", &game_object::root())
        ;

        script::class_<game_object>::type()
        .def("clone", LUA_BIND(&game_object::clone))
        .def("by_tag", LUA_BIND(&game_object::find_by_tag))
        .def("by_index", LUA_BIND(&game_object::child_at))
        .def("set_tag", LUA_BIND(&game_object::set_tag<char const*>))
        .def("parent", LUA_BIND(&game_object::parent))
        .def("add_child", LUA_BIND(&game_object::add_child))
        .def("remove_self", LUA_BIND(&game_object::remove_self))
        .def("remove_all", LUA_BIND(&game_object::remove_all))
        .def("move_up", LUA_BIND(&game_object::move_upward))
        .def("move_top", LUA_BIND(&game_object::move_top))
        .def("move_down", LUA_BIND(&game_object::move_downward))
        .def("move_bottom", LUA_BIND(&game_object::move_bottom))
        .def("add_transform", LUA_BIND(&game_object::add_component<com::transform>))
        .def("get_transform", LUA_BIND(&game_object::get_component<com::transform>))
        ;
        
        script::class_<com::transform>::type()
        .def("set_translate", LUA_BIND((&com::transform::set_translate<float, float, float>)))
        .def("set_rotate", LUA_BIND((&com::transform::set_rotate_by_axis)))
        .def("set_scale", LUA_BIND((&com::transform::set_scale<float, float, float>)))
        .def("set_skew", LUA_BIND_S(com::transform& (com::transform::*)(float, float), &com::transform::set_skew))
        .def("mark", LUA_BIND(&com::transform::mark_dirty))
        ;
    }
}