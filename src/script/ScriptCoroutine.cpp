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

int ScriptCoroutine::GuardYield = 0;
int ScriptCoroutine::GuardReturn = 1;

struct YieldGuard {
	int signature;
	int numOfArg;
	int mask;
	int top;

	YieldGuard(int n, int t, bool s) : numOfArg(n), mask(0), top(t)
	{
		signature = s ? (int)&YieldGuard::startedSignature :
			(int)&YieldGuard::doneSignature;;
		for(int i=0;i<n;++i)
			mask = mask | (1<<i);
	};
	bool test(int n) const { return mask & (1<<n) == 0; };
	void set(n) { mask = mask & (~(1<<n)); }
	bool isClear() const { return mask == 0; }
	bool isValid() const { return doneSignature() || startedSignature(); }
	bool doneSignature() const { return signature == (int)&YieldGuard::doneSignature; }
	bool startedSignature() const { return signature == (int)&YieldGuard::startedSignature; }
};

ScriptCoroutine::ScriptCoroutine(ScriptState const&ss) : _state(ss){
}

bool ScriptCoroutine::resume() {
	lua_State *L = _state.getState();
	int status = lua_status(L);

	if(status == 0) { // a normal state
		// expect a function at the bottom
		//  it won't make sense if there is
		//  extra values under the function
		//  because after resume, the stack
		//  will be cleared out and only re-
		//  return values are kept
		if(!_state.isFunction(1))	// can't resume, drop it
			return true;
		int ret = lua_resume(L, lua_gettop(L) - 1);
		if(ret == 0) {	// the coroutine is finished, leave the stack intact
			return true;
		}else if(ret == LUA_YIELD){
			return false;	// evaluate in the next loop?
		}else{
			// todo: error trace back
			return true;
		}
	}

	if(status == LUA_YIELD) {
	}

	int num = _numArg;
	bool started = hasStarted();
	ASSERT(started || 
			(_state.isFunction(_top + 1) && 
			 _state.top() - _numArg == _top + 1)
			); // fresh run needs a function

	if(!started || (num = pollAndClear()) > 0) {
		// 1. for a fresh state, ... func, varargs
		// 2. for a resuming state, ... events
		int top = _state.top();
		int ret = _state.resume(num); // either fresh start or resume
		if(ret == 0){
			setDone(_state.top() - _top);
			return true;
		}else if(ret == LUA_YIELD){ 
			int n = _state.top() - _top; 
			if(n == 0) // yield return none
				return true; // leave it alone
			else
				setYielded(n);
		}else{
			// todo: track back and error 
			setDone(_state.top() - _top);
			return true; // ditch the error coroutine
		}
	}
	
	return false;
}

int ScriptCoroutine::numReturn(){
	return isDone() ? _state.top() - _top : -1;
}

bool ScriptCoroutine::isGuarded(){
	return _state.isUserdata(-1) &&
		_state.get<YieldGuard>(-1).isValid();
}

bool ScriptCoroutine::isDone(){
	int status = lua_status(_state);

}

bool ScriptCoroutine::hasStarted(){
	return _state.isUserdata(-1) &&
		_state.get<YieldGuard>(-1).startedSignature();
}

void ScriptCoroutine::setYielded(int n){
	ASSERT(n>0);
	ASSERT(!hasStarted());
	_state.push<YieldGuard>(YieldGuard(n, _top, true));		// push the stack info
}

void ScriptCoroutine::setDone(int n){
	ASSERT(n>=0);
	_state.push<YieldGuard>(YieldGuard(n, _top, false));	// push the stack info
}

int ScriptCoroutine::pollAndClear() {
	ASSERT(hasStarted());

	// evaluate every value in the stack
	for(int cur=_state.top(); cur>0; --cur){
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
			if(ret > 0) //todo: error handle?
				_state.pop(ret); // remove any return
		}else{ // for any other value, simply return it
			_state.remove(cur);
		}
	}

	return _state.top() == 0;
}

template<> ScriptState::push_<ScriptCoroutine>(){
	lua_newthread(_L);
}

