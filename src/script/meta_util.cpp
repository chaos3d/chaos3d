/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "script/dispatcher.h"
#include "script/scripting.h"
#include "script/bind.h"
#include "script/lua.h"

#include "core/core.h"
#include "core/RTTI.h"
#include "go/go.h"

_CHAOS_BEGIN

CHAOS_PRIVATE int lua_meta_eq(lua_State* L){
	ScriptData *lhs = (ScriptData *)lua_touserdata(L,1);
	ScriptData *rhs = (ScriptData *)lua_touserdata(L,2);

	if( lhs->obj == 0 && rhs == 0 )
		// nil value or false
		return lua_isnil(L,2) || (lua_isboolean(L,2) && lua_toboolean(L,2) == 0);
	else return (rhs != 0 || lhs->dataType == rhs->dataType ||
		lhs->obj == rhs->obj);
}

CHAOS_PRIVATE int script_shuffle(lua_State* L){
	luaL_argcheck(L, lua_istable(L,1), 1, "Expected a table");
	luaL_argcheck(L, lua_gettop(L) == 1, 2, "No more than 1 argument");
	int n = luaL_getn(L, 1);
	
	for( int i=1;i<n; ++i){
		lua_rawgeti(L, 1, n-i+1);
		int swap = rand()%(n-i)+1;
		lua_rawgeti(L, 1, swap);
		lua_rawseti(L, 1, n-i+1);
		lua_rawseti(L, 1, swap);
	}
	return 1;	// return the table
}

_CHAOS_END