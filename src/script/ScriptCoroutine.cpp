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

bool ScriptCoroutine::resume() {
	int num = _numArg;
	if(!isYielded() || (num = pollAndClear()) > 0) {
		int top = _state.top();
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
		}
	}
	
	return false;
}

bool ScriptCoroutine::isYielded(){
	return _state.isLightuserdata(-1) &&
		_state.getObject(-1) == &GuardYield;
}

void ScriptCoroutine::setYielded(int n){
	ASSERT(n>0);
	ASSERT(!isYielded());
	_state.pushLightuserdata(&GuardReturn);
	for(;n>0;--n)
		_state.push<bool>(false);
	_state.pushLightuserdata(&GuardYielded);
}

void ScriptCoroutine::pollEvt(int num, int cur, int flag, bool flat){
	if(flat){
		_state.push<bool>(true);
		_state.replace(flag);

		_state.remove(cur); // cur == firstEvnt
		for(int i=0;i<num;++i){
			_state.insert(cur);
		}
	}else{
		_state.push<bool>(true);
		_state.replace(flag);
		_state.replace(cur);
		_state.pop(num - 1);
	}
}

int ScriptCoroutine::pollAndClear() {
	ASSERT(isYielded());
	int n = 1;
	int top = _state.top();
	while(!_state.isLightuserdata(top - n))
		++n;
	int firstEvnt = top - 2*(n+1);
	for(int i=0;i<n;++i){
		int cur = firstEvnt + i;
		int flag = firstEvnt + i + n + 1;
		if(_state.get<bool>(flag))
			continue;
			
		if(_state.isFunction(cur)){
			int ret = _state.call(cur, 0);
			if(!_state.get<bool>(-1)){
				_state.pop(ret);
				continue;
			}
			pollEvt(ret, cur, flag, n==1);
		}else if(_state.isCoroutine(cur)){
			ScriptCoroutine sc = _state.get<ScriptCoroutine>(cur);
			if(!sc.isDone())
				continue;
			pollEvt(sc.numReturn(), cur, flag, n == 1);
		}else{
			_state.push<bool>(true);
			_state.replace(flag);
		}
	}
	bool all = true;
	for(int i=0;all && i<n;++i){
		all = _state.get<bool>(-2-i);
	}

	if(all){
		_state.pop(n+2);
		return n;
	}else{
		return 0;
	}
}
