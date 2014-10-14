#ifndef _CHAOS3D_PLATFORM_LUA_MODULE_H
#define _CHAOS3D_PLATFORM_LUA_MODULE_H

#include "lua.hpp"

extern "C" {
    int luaopen_chaos3d(lua_State *L);
}

#endif
