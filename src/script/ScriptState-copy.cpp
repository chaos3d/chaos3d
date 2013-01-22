/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "ScriptState.h"

//using namespace chaos;

///----------------------------------------------------------------
///----------------------------------------------------------------

/**
 * copy one object at index i from one state to another
 * at index cache is a table caching the intermediate
 * object in state 'to' to avoid duplicate copies
 *
 * credit to lualanes
 */
static bool inter_copy_table(lua_State* from, lua_State* to, int cache, int i);
static bool inter_copy_func(lua_State* from, lua_State* to, int cache, int i);
static bool inter_copy_userdata(lua_State* from, lua_State* to, int cache, int i);
static bool inter_copy_one(lua_State* from, lua_State* to, int cache, int i){
	//int top_to = lua_gettop(to);
	ASSERT(lua_istable(to, cache));
	if(lua_checkstack(to, 2) != 1)
		return false; // todo: output errors

	bool ret = true;
	switch(lua_type(from, i)){
		case LUA_TNIL:
			lua_pushnil(to);
			break;
		case LUA_TBOOLEAN:
			lua_pushboolean(to, lua_toboolean(from, i));
			break;
		case LUA_TLIGHTUSERDATA:
			lua_pushlightuserdata(to, lua_touserdata(from, i));
			break;
		case LUA_TNUMBER:
			lua_pushnumber(to, lua_tonumber(from, i));
			break;
		case LUA_TSTRING:
		{
			size_t len; 
			const char *s = lua_tolstring(from, i, &len);
			lua_pushlstring(to, s, len);
		}
			break;
		case LUA_TTABLE:
			ret = inter_copy_table(from, to, cache, i);
			break;
		case LUA_TFUNCTION:
			ret = inter_copy_func(from, to, cache, i);
			break;
		case LUA_TUSERDATA:
			ret = inter_copy_userdata(from, to, cache, i);
			break;
		case LUA_TTHREAD:
			ret = false;
			break;
	};
	
	return ret;
}

//--- copy helper

// return true and push the cached table if already copied over
// or, false and a branch new table
static bool push_cache_table(lua_State* from, lua_State* to, int cache, int i){
	ASSERT(lua_istable(to, cache));
	bool ret = true;

	lua_pushlightuserdata(to, (void*)lua_topointer(from, i));
	lua_pushvalue(to, -1);		// ...cache... id(key), id(key)
	lua_rawget(to, cache);

	if(lua_isnil(to, -1)){		// ... id(key), nil
		lua_pop(to, 1);
		lua_newtable(to);		// ... id, table
		lua_pushvalue(to, -1);	// ... id, table, table
		lua_insert(to, -3);		// ... table, id, table
		lua_rawset(to, cache);
		ret = false; // brand new 
	}else{						// ... id(key), table
		lua_remove(to, -2);
	}
	
	return ret;
}

static bool inter_copy_table(lua_State* from, lua_State* to, int cache, int i){
	//int from_top = lua_gettop(from);

	ASSERT(lua_istable(from, i)); // sanity check
	if(push_cache_table(from, to, cache, i)){
		ASSERT(lua_istable(to, -1));
		return true;
	}

	ASSERT(lua_istable(to, -1)); // load an empty table to fill

	bool ret = true;

	lua_pushnil(from);
	while(lua_next(from, i)){
		int val = lua_gettop(from);
		int key = val - 1;
		if(inter_copy_one(from, to, cache, key)){
			if(inter_copy_one(from, to, cache, val)){
				lua_rawset(to, -3);
			}else{
			}
		} // todo: error check
		lua_pop(from, 1);
	};

	// copy meta table
	if(lua_getmetatable(from, i)){
		if(inter_copy_table(from, to, cache, lua_gettop(from))){
		}else{
			ret = false;
		}
	}

	return ret;
}

// push the key and func, or only the key if func is not cached
// or nil if func not able to be casted (JITed?)
static bool push_cached_func(lua_State* from, lua_State* to, int cache, int i){
	ASSERT(lua_istable(to, cache));
	void* key = (void*)lua_topointer(to, i);
	if(key == NULL) { // nil? todo: error output 
		lua_pushnil(to);
		return false;
	}
	lua_pushlightuserdata(to, key);
	lua_pushvalue(to, -1);
	lua_rawget(to, cache);
	if(lua_isnil(to, -1)) {
		lua_pop(to, 1);
		return false;
	}
	ASSERT(lua_isfunction(to, -1));
	return true;
}

static int buf_writer( lua_State *L, const void* b, size_t n, void* B ) {
	luaL_addlstring((luaL_Buffer*) B, (const char *)b, n);
	return 0;
}

static bool inter_copy_func(lua_State* from, lua_State* to, int cache, int i){
	ASSERT(lua_isfunction(from, i));
	if(push_cached_func(from, to, cache, i)){
		lua_remove(to, -2); // remove the key
		return true;
	}
	
	int to_top = lua_gettop(to);
	bool ret = true;
	bool needToPush = (i != lua_gettop(from));
	if(needToPush)
		lua_pushvalue(from, i);

	luaL_Buffer b;
	luaL_buffinit(from, &b);
	lua_CFunction cfunc = NULL;
	if(lua_dump(from, buf_writer, &b) == 0) {
		luaL_pushresult(&b);
		size_t sz;
		char const* s = lua_tolstring(from, -1, &sz);
		if(!luaL_loadbuffer(to, s, sz, NULL)){
			ret = false;	// todo: error out
		}else{
			// cache the function
									// ... key, function
			lua_insert(to, -2);		// ... function, key
			lua_pushvalue(to, -2);	// ... function, key, function
			lua_rawset(to, cache);	// ... function
		}
		lua_pop(from, 1); // pop the compilied source
	}else{ // c function?
		cfunc = lua_tocfunction(from, i);
		ret = cfunc != NULL;
		// c function won't be cached because the closure could be different
		lua_pop(to, 1); // pop th key
	}

	if(needToPush)
		lua_pop(from, 1);

	// copy the upvalues
	int j = 0;
	for(;ret && lua_getupvalue(from, i, j+1) != NULL; ++j) {
		ret = inter_copy_one(from, to, cache, lua_gettop(from));
		lua_pop(from, 1);
	}

	if(ret == false){
		return false;
	}

	if(cfunc != NULL){
		lua_pushcclosure(to, cfunc, j);
	}else{
		for(;j>0;--j){
			lua_setupvalue(to, to_top + 1, j);
		}
	}

	ASSERT(lua_isfunction(to, -1));
	return true;
}

static bool inter_copy_userdata(lua_State* from, lua_State* to, int cache, int i){
	// todo
	lua_pushnil(to);
	return true;
}

bool ScriptState::copy(ScriptState const& from, int n){
	if(lua_checkstack(_L, n + 1) != 1) // n objects + cache table
		return false;

	bool ret = true;
	int top1 = from.top(), top2 = top();

	lua_newtable(_L);
	for(int i = 0;i < n; ++i){
		if(!inter_copy_one(from, _L, top2+1, top1+i)){
			ret = false;
			break;
		}
	}
	if(ret){
		lua_remove(_L, top2+1); // remove cache table
	}else{
		lua_settop(_L, top2); // pop everything on errors
	}
	return ret;
}
