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

ScriptCoroutine::ScriptCoroutine(ScriptState const&ss) : _state(ss), _numArg(0){
	if(!isGuarded()){ // a fresh state: nothing at the top
		_top = _state.top();
		return;
	}

	// cache to local
	YieldGuard const& guard = _state.get<YieldGuard>(-1);
	_top = guard.top;
	_numArg = guard.numOfArg;
}

bool ScriptCoroutine::resume() {
	if(isDone())
		return true;

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
	return _state.isUserdata(-1) &&
		_state.get<YieldGuard>(-1).doneSignature();
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

	YieldGuard& guard = _state.get<YieldGuard>(-1);
	int num = guard.numOfArg;
	int top = _state.top();
	int firstEvt = top - 1 - num;
#if 0
	// todo: unpack all returns for one event
	if(num == 1){
	}else
#endif
	for(int i=0;i<num;++i){
		int cur = firstEvnt + i;
		if(guard.test(i))
			continue;
			
		if(_state.isFunction(cur)){	// function as a polling method
			int ret = _state.call(cur, 0);
			if(ret > 0)
				if(!_state.get<bool>(-ret)){ // check the first one
					_state.pop(ret);
					continue;
				}else{	// a 'true' value
					guard.set(i);
					_state.pop(ret - 1);	// only save one return
					_state.insert(cur);
					_state.remove(cur+1);	// remove the function
				}
			}else if(ret == 0){
				// nothing returns, ignore and keep waiting
			}
		}else if(_state.isCoroutine(cur)){	// wait until coroutine is finished
			ScriptCoroutine sc = _state.get<ScriptCoroutine>(cur);
			if(sc.isDone()){
				guard.set(i);
				int ret = sc.numReturn();
				if(ret > 0){
					_state.pop(ret - 1);	// only save the first return
				}else{
					_state.pushnil();		// if coroutine returns nothing, push nil placeholder
				}
				_state.insert(cur);
				_state.remove(cur+1);		// remove the coroutine
			}
		}else if(_state.isUserdata(cur)){ // call metamethod __block if any, or push nil and resume
			int ret = _state.callmeta(cur, "__block");
			if(ret <= 0) { // no meta table or meta method found or no return
				guard.set(i);
			} else {
				if(!_state.get<bool>(-ret)){ // should not block?
					guard.set(i);
				}
				_state.pop(-ret);
			}
		}else{ // for any other value, simply return it
			guard.set(i);
		}
	}

	if(guard.isClear()){
		_state.pop(1); // pop the stack info
		return num;
	}else{
		return 0;
	}
}

template<> ScriptState::push_<ScriptCoroutine>(){
	lua_newthread(_L);
}

