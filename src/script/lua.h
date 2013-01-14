/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef _CHAOS_SCRIPT_LUA_H
#define _CHAOS_SCRIPT_LUA_H

extern "C"{
#include "../../external/luajit/lua.h"
#include "../../external/luajit/lauxlib.h"
#include "../../external/luajit/lualib.h"
}

// helper marco for popping values
#define	LUA_GET1(L)		lua_rawgeti(L, -1, 1 )
#define	LUA_GET2(L)		lua_rawgeti(L, -1, 1 ); lua_rawgeti(L, -2, 2 )
#define	LUA_GET3(L)		lua_rawgeti(L, -1, 1 ); lua_rawgeti(L, -2, 2 ); lua_rawgeti(L, -3, 3 )
#define	LUA_GET4(L)		lua_rawgeti(L, -1, 1 ); lua_rawgeti(L, -2, 2 ); lua_rawgeti(L, -3, 3 ); lua_rawgeti(L, -4, 4 )

#endif