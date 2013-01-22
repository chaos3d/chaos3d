/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_BIND_H
#define	_CHAOS_BIND_H

#include "core/RTTI.h"

#include "FunctionDeduce.h"
#include "LuaFactory.h"

class Bind {
public:
    
    static void loadmeta(lua_State *L) {
        static luaL_Reg[] entries = {
            {NULL, NULL},
        };
        
        newmetatable(&entries);
        __index,
    }
};

ScriptState::push_<Clz>(Clz* A){
    A->retain();
    lua_pushuserdata(L, A);
    loadmeta();
    lua_setmetatable();
}
#endif

