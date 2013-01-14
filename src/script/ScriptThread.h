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
a running state of a specific script code
*/
DECLARE_CLASS(ScriptThread, Triggerable);
class CHAOS_API ScriptThread : public Triggerable, public ScriptState{
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

