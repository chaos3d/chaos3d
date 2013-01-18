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
			lua_settop(0);
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
		if(_state.isFunction(cur)){	// function as a polling method
			_state.push(cur);		// push the function
			int ret = _state.call(0);
			if(ret > 0){
				if(!_state.get<bool>(-ret)){// check the first one
					_state.pop(ret);
					continue;
				}else{	// a 'true' value
					_state.pop(ret);		// pop all results
					_state.remove(cur);		// remove the function
				}
			}else if(ret == 0){
				// nothing returns, ignore and keep waiting
			}else{ // error?
				// todo: trace back
				_state.pop(1);		// pop the error
				_state.remove(cur);	// remove the function
			}
		}else if(_state.isCoroutine(cur)){	// wait until coroutine is finished
			ScriptCoroutine sc = _state.get<ScriptCoroutine>(cur);
			if(sc.isDone()){
				_state.remove(cur);			// remove the coroutine
			}
		}else if(_state.isUserdata(cur)){ 	// call metamethod __block if any
			int ret = _state.callmeta(cur, "__block");
			if(ret <= 0 ||  // no meta table or meta method found or no return
				!_state.get<bool>(-ret)){ // should not block?
				_state.remove(cur);
			}
			if(ret > 0)
				_state.pop(ret); // remove any return
			//todo: error handle?
		}else{ // for any other value, simply return it
			_state.remove(cur);
		}
	}

	return _state.top() == 0;
}

template<> ScriptState::push_<ScriptCoroutine>(){
	lua_newthread(_L);
}

