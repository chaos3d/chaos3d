/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "ScriptCoroutine.h"

int ScriptCoroutine::GuardYield = 0;
int ScriptCoroutine::GuardReturn = 1;

struct YieldGuard {
	int numOfArg;
	int mask;
	int top;
	YieldGuard(int n, int t) : numOfArg(n), mask(0), top(t)
	{
		for(int i=0;i<n;++i)
			mask = mask | (1<<i);
	};
	bool test(int n) { return mask & (1<<n) == 0; };
	void set(n) { mask = mask ^ (1<<n); }
	bool isClear() { return mask == 0; }
};

ScriptCoroutine::ScriptCoroutine(ScriptState const&ss) : _state(ss){
	if(!isYielded()) // a fresh state
		return;

	// cache to local
	YieldGuard const& guard = _state.get<YieldGuard>(-2);
	_top = guard.top;
	_numArg = guard.numOfArg;
}

bool ScriptCoroutine::resume() {
	int num = _numArg;
	if(!isYielded() || (num = pollAndClear()) > 0) {
		int top = _top = _state.top();
		int ret = _state.resume(num);
		if(ret == 0){
			setStatus(Done);
			return true;
		}else if(ret == LUA_YIELD){ 
			int n = _state.top() - top;
			if(n == 0) // yield return none
				return true; // leave it alone
			else
				setYielded(n);
		}else{
			// todo: track back and error 
			return true; // ditch the error coroutine
		}
	}
	
	return false;
}

int ScriptCoroutine::numReturn(){
	if(isYielded())
		return -1;
	else
		return _state.top() - _top;
}

bool ScriptCoroutine::isDone(){
	return true; //todo
}

bool ScriptCoroutine::isYielded(){
	return _state.isLightuserdata(-1) &&
		_state.getObject(-1) == &GuardYield;
}

void ScriptCoroutine::setYielded(int n){
	ASSERT(n>0);
	ASSERT(!isYielded());
	_state.push<YieldGuard>(YieldGuard(n, _top));	// push the stack info
	_state.pushLightuserdata(&GuardYielded);		// push the signature
}

int ScriptCoroutine::pollAndClear() {
	ASSERT(isYielded());

	YieldGuard& guard = _state.get<YieldGuard>(-2);
	int num = guard.numOfArg;
	int top = _state.top();
	int firstEvt = top - 2 - num;
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
		_state.pop(2); // pop the signature and info
		return num;
	}else{
		return 0;
	}
}
