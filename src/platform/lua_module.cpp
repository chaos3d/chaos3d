#include "lua_module.h"

#include "re/render_device.h"

#include "script/type/def.h"
#include "script/class_type.h"
#include "script/lua_bind.h"
#include "script/state.h"
#include "script/lua_ref.h"
#include "script/import_scope.h"
#include "script/converter_ptr.h"

#include "platform/launcher.h"

#include <log4cxx/basicconfigurator.h>
#include <array>

IMPORT_CLASS_TYPE(render_device);
IMPORT_CLASS_TYPE(native_window);
IMPORT_CLASS_TYPE(root_action);

using namespace script;

extern "C" void c3d_lua_import(lua_State *L);
extern "C" int luaopen_chaos3d(lua_State *L) {
    auto checker = &launcher::initialize;
    if (checker == nullptr) {
        luaL_error(L, "core lib isn't being loaded.");
        return 0;
    }
    
    log4cxx::BasicConfigurator::configure();
    log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getAll());
    
    c3d_lua_import(L);
    
    auto state = state::create(L);
    state->import("chaos3d")
    .def("init", LUA_BIND(&launcher::initialize))
    .def_singleton_getter<launcher>("get_launcher")
    ;
    
    class_<launcher>::type()
    .def("create_game_window", LUA_BIND(&launcher::create_game_window))
    .def("poll_event", LUA_BIND(&launcher::poll_event))
    .def("get_render_device", LUA_BIND(&launcher::get_render_device))
    .def("get_action", LUA_BIND(&launcher::action))
    ;
    
    lua_getglobal(L, "chaos3d");
    return 1;
}
