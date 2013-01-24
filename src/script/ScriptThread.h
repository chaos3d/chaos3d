/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_SCRIPTTHREAD_H
#define	_CHAOS_SCRIPTTHREAD_H

#include "common.h"
#include "LuaType.h"
#include "ScriptState.h"
#include "ScriptCoroutine.h"

_CHAOS_BEGIN

/**
 * This is a wrapper of the state in a 'thread'.
 * it basically provides API to communicate between
 * threads(states) using messaging, and maintains
 * coroutines to simplify the run loop.
 *
 * thread/coroutine/manager won't interfere the
 * state, it is a tool to schedule existing 
 * coroutines and utilize system threads to 
 * implement multi-threading. so there shouldn't
 * be any function relating to the state
 *
 * -- TODO
 * A script thread is a thread that runs Lua script.
 * it'll handle coroutines (every code runs as a 
 * coroutine) and polling (yielding will poll the
 * state). it should be an independent virtual
 * machine, and communication between script threads
 * is by piping/messaging (see LuaLanes). when an
 * object is being passed, it'll look for the meta
 * method '__retain' (the internal object, i.e. 
 * table can't be retained but copied because it
 * can't be shared among different vm's.
 */

class CHAOS_API ScriptThread{
public:
	static ScriptState	NilState;

public:
    ScriptThread();
    ScriptThread(ScriptState const&);
    ~ScriptThread();
    
    /**
     * send message to the thread, non-blocking if queued
     */
    bool send(LuaType const& msg, bool queued = true);
    
    /**
     * receive message from the thread
     * blocked until there is something if the return 
     * won't allow to be nil
     * to access the lua ref, the hosted thread should be locked
     */
    LuaType receive(bool couldBeNil = true);
    
	/**
	* dispose a thread
	*/
	void dispose();

	/**
	 * schedule the coroutines
	 * returns true if all the coroutines are finished
	 *
	 * the guarded coroutines are saved in registry table
	 */
	bool loop();

	/**
	 * add the coroutine to the list so it'll run
	 * in next loop
	 *
	 * a.k.a spawn
	 */
	void schedule(ScriptCoroutine const&);
    ScriptCoroutine schedule(ScriptState::SourceReader const&);

	// todo
	// - coroutine.resume (remove from the waiting list)
	// - coroutine.spawn
	// - coroutine.schedule
	// - threaded states
	static ScriptThread* newThread(char const* script);

    ScriptState& getState() { return _state; }
    
private:
    void setupState();
    
	ScriptState _state;
    //LuaType _coresume;  // coroutine.resume
};

//TYPE_RET_REF( ScriptThread );

_CHAOS_END

#endif
