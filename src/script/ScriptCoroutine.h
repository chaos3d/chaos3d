/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_SCRIPTCOROUTINE_H
#define	_CHAOS_SCRIPTCOROUTINE_H

#include "common.h"
#include "ScriptState.h"

_CHAOS_BEGIN

/**
 * a coroutine wrapper that manages the yields
 * stack so it can be communicated with the host
 * by yielding
 */
class ScriptCoroutine {
public:
	enum Status{
		Created,
		Yielded,
		Done
	};

public:
	ScriptCoroutine(ScriptState const&, int numArg);
	~ScriptCoroutine();

	/**
	 * run/resume the script
	 * false if the thread needs to be re-added to the list to run
	 * true can be an non-recoverable error in the script
	 *
	 * if the script yields:
	 * a function(c/lua): it will test if the return is the true once a time
	 * a coroutine: it will try to run the coroutine until it finishes
	 * none: it will be yielded until readded by others
	 * any value: it will re-add to the list
	 *
	 * if the script yields multiple returns, it'll wait until
	 * all conditions satisify; and they need to be either functions
	 * or coroutines
	 */
	bool resume();

	// in the stack of a coroutine, a guard of light userdata 
	// is at top 
	bool isYielded();
	
	// test if the coroutine is the main thread in the state
	bool isMain();

	// test if all events are over
	bool isReady();

	int numReturn();
private:
	void setYielded(int n);

	// poll all the events and may clear the guard
	int pollAndClear();
	void pollEvt(int num, int cur, int flag, bool flat);

	ScriptState _state;	// coroutine is a dependent state
	int _numArg;

	static int GuardReturn, GuardYield;
};

_CHAOS_END
#endif
