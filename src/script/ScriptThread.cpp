/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "script/ScriptThread.h"

#include "core/core.h"
#include "script/dispatcher.h"
#include "script/ScriptData.h"
#include "script/LuaType.h"
#include "script/ScriptManager.h"

//using namespace chaos;

IMPLEMENT_CLASS(ScriptThread, LIB_DOMAIN )
IMPLEMENT_END;

ScriptState ScriptThread::NilState(0);

#ifdef DEBUG
ScriptThread::TThList ScriptThread::threads;
#endif
///----------------------------------------------------------------
///----------------------------------------------------------------

#ifdef DEBUG
void ScriptThread::release(){
	ReferencedCount::release();
}

void ScriptThread::retain(){
	ReferencedCount::retain();
}
#endif

ScriptThread* ScriptThread::thread(TypeLua const& lua){
	if( lua.getRef() < 0 )
		return 0;

	ScriptThread* ss(ScriptManager::getInstance()->createThread());

	lua.get(ss->getState());
	if( !lua_isfunction( ss->getState(), 1 ) ){
		LOG("Not an available script code.");
		ScriptManager::getInstance()->recycleThread(ss);		
		return 0;
	}

	// function is at index of 1
	return ss;
}

bool ScriptThread::run(bool immediate){
	if( immediate )
		return ScriptManager::getInstance()->run( this );
	else
		this->wait4( );//start signal
	return true;
}

void ScriptThread::dispose(){
	ScriptManager *sm(ScriptManager::getInstance());
	if( this == sm->current() ){
		LOG("Unable to terminate self thread");
		return;
	}else if( mStatus == Terminated){
		// a dead thread
		return;
	}

	// release the thread
	setThis(TypeLua());
	mStatus = Terminated;

	// pulse the thread to let waiting list run
	Sequence::getInstance()->activate( this );		// thread is a mutex

	if( refCount() == 1 ){	// no other references, recycle it
		sm->recycleThread( this );
	}else
		release();
}

/*
void ScriptThread::wait(ScriptThread* ss){
	mWaitList.push_back( ss );

	ss->mMutexList.push_back( WaitObject(this) );
	this->retain();
}
*/

ScriptThread::~ScriptThread(){
	// released? try to recycle lua state
	ScriptManager *sm(ScriptManager::getInstance());
	if( mStatus != None && lua_status(mL) == 0 && sm != 0){
		sm->recycleThread(new ScriptThread(mL, mRef));
	}

#ifdef DEBUG
	threads.remove(this);
#endif
}
