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
 *
 * this makes some assumptions for how the co-
 * routine should yield and interact. Only guarded 
 * coroutines will follow the rules. So there
 * are the ways to guard a coroutine:
 * 1. coroutine.spawn(func)
 * 2. yield a coroutine in a guarded one
 * 	(any coroutine is yielded from a guarded one
 * 	will run in the guarded mode.
 * 3. spawn from native c++ and schedule to the
 * 	thread
 * 
 * basically, the guarded coroutine will run in
 * a thread where it handles how to yield and
 * resume. any coroutine can turn into guarded
 * mode, but a guarded coroutine that if yielded 
 * will be resumed in the non-guarded mode will
 * cause the system vulnerable because the yielded/
 * suspended guarded coroutine is added in the
 * waiting/polling list, and thus will be resumed
 * again
 *
 * --------
 * The first thought was to have the returns from
 * the "polling functions", which will make asum-
 * ptions on the stack. This will cause inconsit-
 * ensy with the native coroutines, and thus lose 
 * compatibility. So, the coroutine wrapper could
 * presumably wrap any coroutine (either yielded
 * one or a new one), the only thing it'll do is
 * to frequently check trueness of the yielded 
 * values and resume the coroutine. If the coroutine
 * is resumed not by the scheduler, it's fine and
 * it'll be automatically removed from the scheduler
 *
 */
class ScriptCoroutine {
public:
	enum Status{
		Created,
		Yielded,
		Done
	};

public:
	// construct the wrapper from the state
	ScriptCoroutine(ScriptState const&);
	~ScriptCoroutine();

	/**
	 * run the coroutine if it is in a normal state
	 * and it has the function at the bottom of the 
	 * stack 
	 *
	 * resume the coroutine if it is yielded and
	 * all the values in the stack are cleared
	 *
	 * returns:
	 * false if the thread needs to be re-added to the list to run
	 * true can be an non-recoverable error in the script
	 *
	 * if the script yields:
	 * a function(c/lua): it will test if the return is the true once a time
	 * a coroutine: it will try to run the coroutine until it finishes
	 * a userdata: it will call meta __block
	 * none: it will be yielded until readded by others
	 * any value: it will re-add to the list
	 *
	 * if the script yields multiple returns, it'll wait until
	 * all conditions satisify; and they need to be either functions
	 * or coroutines or userdata with meta __block
	 *
	 * once the stack is cleared, the coroutine will be automatically
	 * resumed by the scheduler(ScriptThread)
	 *
	 * when the coroutine is ended normally, the stack will
	 * get cleared out.
	 */
	bool resume();

	/**
	 * the coroutine is done if 
	 *  - it's not yielded and
	 *  - the stack is empty
	 */
	bool isDone();

	// test if the coroutine is the main thread in the state
	bool isMain();

	ScriptState& getState() { 
		return _state; 
	}

	ScriptState const& getState() const { 
		return _state; 
	}

private:

	// poll all the events and may clear the guard
	bool pollAndClear();

	ScriptState _state;	// coroutine is a dependent state

};

template<> ScriptState::push_<ScriptCoroutine>();

_CHAOS_END
#endif
