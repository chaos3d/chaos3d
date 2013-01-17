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

#include "chaos_config.h"
#include "core/core.h"
#include "core/RTTI.h"
#include "ScriptState.h"
#include "ScriptManager.h"

_CHAOS_BEGIN

/*
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

DECLARE_CLASS(ScriptThread, NilParent);
class CHAOS_API ScriptThread{
	friend class ScriptManager;
	//DYNAMIC_CLASS;

	//DECLARE_NOTCOPYABLE(ScriptState);
	DECLARE_NOTASSIGNABLE(ScriptThread);

public:
	static ScriptState	NilState;

	enum{
		None,
		Created,
		Running,
		Yielded,
		CYielded,	// yielded from a c function
		Terminated,
	};

	typedef std::list<ScriptThread*> TThList;

#ifdef	DEBUG
	static TThList	threads;
#endif

protected:
    ScriptState mState; // todo: need to lock
	TypeLua		mRef;			// the reference to the thread
	TypeLua		mThis;
	int			mStatus;

	/*
	Created by script manager with the function on the stack
	*/
	// call create instead
	ScriptThread( lua_State* L, TypeLua const& lua)
		:ScriptState(L), mRef(lua), mThis(-1), mStatus(None){
#ifdef	DEBUG
		threads.push_back(this);
#endif
	};

	// call dispose instead
	virtual ~ScriptThread();

public:
    /**
     * send message to the thread, non-blocking if queued
     */
    bool send(TypeLua const& msg, bool queued = true);
    
    /**
     * receive message from the thread
     * blocked until there is something if the return 
     * won't allow to be nil
     * to access the lua ref, the hosted thread should be locked
     */
    TypeLua receive(bool couldBeNil = true);
    
    /**
     * load the source
     */
    bool load(ScriptState::SourceReader& reader);
    
    /**
     * run the vm
     * return false if every coroutine has been done
     * and the vm can be reloaded and resued
     * this should only be called in the same thread as
     * it's being intialized.
     */
    bool run();
    
    /**
     *
     */
    ScriptThread* spawn(ScriptState::SourceReader& reader);
    
#ifdef DEBUG
	virtual void	release();
	virtual void	retain();
#endif


	explicit ScriptThread( lua_State* L )
		:ScriptState(L),  mStatus(None){
	};

	inline
	int getStatus() const {
		return mStatus;
	};

	// reference as a this pointer
	inline
	ScriptThread* setThis( TypeLua const& lua ){
		mThis = lua;
		return this;
	}

	// pop the top element as a this pointer
	inline
	ScriptThread* setThis( ){
		mThis = TypeLua( mL );
		return this;
	}

	/**
	* create a thread using a function ref
	*/
	static ScriptThread* thread(TypeLua const&);

	/**
	 * current running thread
	 */
	static ScriptThread* current(){
		return ScriptManager::getInstance()->current();
	};

	/**
	* run a thread
	* @param
	*	imemediate, true if run immediately, false if add to the waiting list until next loop
	* @return
	*	true, if ends or errors
	*	false, if yields
	*/
	bool	run( bool immediate );

	/**
	* dispose a thread
	*/
	void	dispose();

	// run the thread when activated
	virtual void activated(){
		ScriptManager::getInstance()->run( this );
	};

	/**
	* callback when the thread is done
	*/
	//void	addTriggerable(TCallBack callback, void* userdata){
	//	mMutexList.push_back( WaitObject((void*)callback,userdata,WaitObject::Callback) );
	//};

	/**
	 * help methods for writing "streaming" code, like
	 * ScriptThread::thread(func)->push_()->push_()->run()
	 */
	template<class T>
	inline ScriptThread* push_(T val){
		ScriptState::push_<T> (val) ;
		return this;
	};
};

TYPE_RET_REF( ScriptThread );

_CHAOS_END

#endif

