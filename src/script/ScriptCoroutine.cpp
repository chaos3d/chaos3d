/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "ScriptCoroutine.h"
#include "lua.h"

ScriptCoroutine::ScriptCoroutine(ScriptState const&ss) : _state(ss){
}

bool ScriptCoroutine::resume() {
	lua_State *L = _state.getState();
	int status = lua_status(L);

	if(status == 0 && !lua_isfunction(_state, 1))
		return true;	// expect a funciont on a regular coroutine
						// drop it
	else if(status == 0 ||	// about to start
			(status == LUA_YIELD && pollAndClear())){ // yielded but all events are cleared
		// function at the bottom for the first time
		int ret = lua_resume(L, lua_gettop(L) - (status == 0 ? 1 : 0));
		if(ret == 0) {	
			// the coroutine is finished and
			// clear the stack because they 
			// won't go anywhere and an indicator
			// that it is finished
			lua_settop(L, 0);
			return true;
		}else if(ret == LUA_YIELD){
			return false;	// evaluate in the next loop?
		}else{
			// todo: error trace back
		}
	}
	// error? drop it
	return true;	
}

bool ScriptCoroutine::isDone(){
	int st = lua_status(_state);
	if(st == LUA_YIELD)
		return false;
	else if(st == 0)
		return lua_gettop(_state) == 0;
	else
		return true;	// todo: error?
}

bool ScriptCoroutine::pollAndClear() {
	// evaluate every value in the stack
	for(int cur = lua_gettop(_state); cur>0; --cur){
		if(lua_isfunction(_state, cur)){	// function as a polling method
			lua_pushvalue(_state, cur);		// push the function
			int ret = lua_pcall(_state, 0, LUA_MULTRET, 0); // TODO: error check
			if(ret > 0){
				if(!_state.get_<bool>(-ret)){// check the first one
					_state.pop(ret);
					continue;
				}else{	// a 'true' value
					_state.pop(ret);		// pop all results
					lua_remove(_state, cur);		// remove the function
				}
			}else if(ret == 0){
				// nothing returns, ignore and keep waiting
			}else{ // error?
				// todo: trace back
				_state.pop(1);		// pop the error
				lua_remove(_state, cur);	// remove the function
			}
		}else if(lua_isthread(_state, cur)){	// wait until coroutine is finished
			ScriptCoroutine sc = _state.get_<ScriptCoroutine>(cur);
			if(sc.isDone()){
				lua_remove(_state, cur);			// remove the coroutine
			}
		}else if(lua_isuserdata(_state, cur)){ 	// call metamethod __block if any
			int ret = luaL_callmeta(_state, cur, "__block");
			if(ret <= 0 ||  // no meta table or meta method found or no return
				!_state.get_<bool>(-ret)){ // should not block?
				lua_remove(_state, cur);
			}
			if(ret > 0)
				_state.pop(ret); // remove any return
			//todo: error handle?
		}else{ // for any other value, simply return it
			lua_remove(_state, cur);
		}
	}

	return _state.top() == 0;
}

template<> void ScriptState::push_<ScriptCoroutine>(){
	lua_newthread(_L);
}

