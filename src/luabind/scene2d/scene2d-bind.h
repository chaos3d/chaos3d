#ifndef _SCENE2D_BIND_H
#define _SCENE2D_BIND_H

#include <lua/lua.hpp>

#if defined(__cplusplus)
extern "C" {
#endif

int luaopen_scene2d(lua_State*);

#if defined(__cplusplus)
}
#endif

#endif