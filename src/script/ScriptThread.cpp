/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "ScriptThread.h"

//using namespace chaos;

///----------------------------------------------------------------
///----------------------------------------------------------------

static int _thread_schedule_sig = 0;

void ScriptThread::schedule(ScriptCoroutine const&sc){
	lua_pushlightuserdata(_state, (void*)&_thread_schedule_sig);
	lua_rawget(_state, LUA_REGISTRYINDEX);
	ASSERT(lua_istable(_state, -1));
	lua_pushthread(sc.getState());
    lua_xmove(sc.getState(), _state, 1);
	lua_pushboolean(_state, 1);
	lua_rawset(_state, -3);
	_state.pop(1);
}

bool ScriptThread::loop(){
	lua_pushlightuserdata(_state, (void*)&_thread_schedule_sig);
	lua_rawget(_state, LUA_REGISTRYINDEX);
	//ASSERT(lua_istable(_state, -1));
    if(!lua_istable(_state, -1)){
        lua_pop(_state, 1);
        return true;
    }
    
	int t = lua_gettop(_state);

	lua_pushnil(_state);
	while(lua_next(_state, t) != 0){
		ScriptCoroutine sc = _state.get_<ScriptCoroutine>(-2);
		if(sc.resume()){
			lua_pushvalue(_state, -2);	// table, ..., cr, value, cr
			lua_insert(_state, t+1);	// table, cr, .., cr, value
		}
		lua_pop(_state, 1);
	}

	bool any = true;
	// table, cr, ..., cr
	while(!lua_istable(_state,-1)){
		any = false;
		lua_pushnil(_state);
		lua_rawset(_state, t);
	}
    
    _state.pop(1); // pop the table
    lua_gc(_state, LUA_GCSTEP, 2);
	return any;
}

void ScriptThread::dispose(){
}

void ScriptThread::setupState(){
	lua_pushlightuserdata(_state, (void*)&_thread_schedule_sig);
	lua_rawget(_state, LUA_REGISTRYINDEX);
	if(!lua_istable(_state, -1)){
        lua_pop(_state, 1);
        lua_pushlightuserdata(_state, (void*)&_thread_schedule_sig);
        lua_newtable(_state);
        lua_rawset(_state, LUA_REGISTRYINDEX);
    }
    
    luaL_openlibs(_state);  // todo: only load a certain libs
}

ScriptThread::ScriptThread(){
    setupState();
}

ScriptThread::~ScriptThread(){
}

