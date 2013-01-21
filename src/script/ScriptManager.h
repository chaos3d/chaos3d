/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_SCRIPTMANAGER_H
#define	_CHAOS_SCRIPTMANAGER_H

#include "common.h"
//#include "core/core.h"
//#include "core/RTTI.h"

#include "LuaType.h"

#include <string>
#include <list>
#include <vector>

struct lua_State;
struct lua_Debug;

#define DEPRECATED_SCRIPT(func) do{ LOG( "DEPRECATED " #func "@(%s), %s, (%d)", \
			ScriptManager::getInstance()->currentInfo().c_str(), __FILE__, __LINE__); exit(0); }while(0);

_CHAOS_BEGIN

class Script;
class ScriptThread;

class DataStream;
class DataSource;
class Timer;
class Sequence;

/**
 * this will be the same manager as before except it won't be
 * a singleton and it is thread-based. esentially, it manages
 * coroutines per state per thread.
 */

/*
to handle dispatches between coroutines and other scripting issues.
*/
DECLARE_CLASS(ScriptManager,NilParent);
class CHAOS_API ScriptManager : public Singleton<ScriptManager>{
	DECLARE_NOTCOPYABLE(ScriptManager);
	DECLARE_NOTASSIGNABLE(ScriptManager);

public:	
	class Debugger{
	public:
		virtual ~Debugger(){};
		virtual void line(lua_State*, lua_Debug *){};
		virtual void return_(lua_State*, lua_Debug *){};
		virtual void call(lua_State*, lua_Debug *){};
		virtual void count(lua_State*, lua_Debug *){};
	};
	
	typedef	std::vector<ScriptThread*>	TThPool;

	enum{
		MAX_FORK = 16
	};

	typedef std::vector<ScriptThread*>	TRunningStack;

	struct{
		int		total, totalCreated;
		int		maxPool,numPool;
		int		numForPool, numNotForPool;
		int		maxRun,numCurRun;
	}		stat;

protected:

	GSymbolTable*		mSymbolTbl;
	//TThreadList			mThreads;
	TThPool				mThPool;

	// the global lua state
	lua_State*		mL;

	// the current running thread
	TRunningStack	mRunning;

	// the debugger
	Debugger*		mDebugger;
	
	// the updating script, that will be executed every loop and thus not allowed to be suspended
	TypeLua			mUpdatingScript;

	Timer const&	mTimer;		// global timer
	Sequence const& mSequence;	// global event trigger

	// fork function
	static int	fork(lua_State* L);

	// url function
	static int	url(lua_State* L);

	// timer function
	static int	timer(lua_State* L);

	// timer function
	static int	swait(lua_State* L);

public:
	ScriptManager(Timer const&, Sequence const&);
	~ScriptManager();

	// run the bootstrap scripts
	bool		bootstrap(DataStream* ds);

	// load the symbols' table using namespace 'name'
	void		loadSymbols(const char *name, SymbolTable *table);
	// load a symbol to global space as an instance
	void		loadSymbol(const char *name, void* obj, Type* type);

	// create a script from a stream
	Script*		scriptByData( DataStream* ds );

	// dump a function refered by ref to source
	bool		dump( int ref, DataSource* ds );
	bool		dumpScript( Script* script, DataSource* ds );

	/**
	 * updating script
	 */
	void	setUpdatingScript( TypeLua const& lua ) { mUpdatingScript = lua; };
	TypeLua	getUpdatingScript() const { return mUpdatingScript; };
	
	/**
	 * delete a script
	 */
	void		releaseScript( Script* script);

	// create and schedule a thread
	ScriptThread*	schedule( Script* code );

	/**
	* get current thread
	*/
	ScriptThread*	current( ) {
		return mRunning.back();
	}

	/**
	* run a thread
	* @return
	*	false, if yields, then push into waiting list
	*	true, if ends normally or errors
	*/
	bool run(ScriptThread*);
	bool run(char const*);

	// create a thread to run the code
	/*
	ScriptThread*		createState( int codeRef, bool scheduled = true );
	inline ScriptThread*	createState( TypeLua const& funRef, bool scheduled = true  ){
		return createState( funRef.getRef(), scheduled );
	}*/

	/**
	* create and run a thread by a url
	*/
	ScriptThread*	schedule( char const* );

	// create a thread from a stream
	ScriptThread*	threadByData( DataStream* ds, bool scheduled );

	// terminate a thread
	//void		destroyState(ScriptState* ss);

	// pooled a ternimated thread
	//void		pooledState(ScriptState* ss);

	/**
	* pool a thread
	* @remark
	*	can only pool normal ended thread
	*/
	void	recycleThread( ScriptThread* );

	/**
	* create a thread
	* @remark
	*	will try to reuse threads in the pool, otherwise create new one.
	*/
	ScriptThread*	createThread();

	// run immediately in the current thread
	// that said, lua_pcall
	int			runNow( Script* code, int args );

	// update all the active events
	bool		runScripts();

	/**
	* return current debugging infor
	*/
	std::string	currentInfo();

	/**
	* enable and disable debugging with hooks
	* @param
	*	debugger, disabled if nil
	*/
	void		enableDebug(Debugger*);
	Debugger*	getDebugger() const { return mDebugger; };

	// add an thread to a waiting list
	//void		wait( void* mutex, ScriptState* ss = 0/*0 for current running thread*/);

	void	timer( float t );

	// use this return for c function
	int		return_(int args);

	lua_State*		getState(){
		return mL;
	}

	ScriptThread*	getRunning(){
		return mRunning.back();
	}

	/**
	* dipose a thread, scripting helper
	*/
	void	dispose(ScriptThread* ss);
};

_CHAOS_END

#endif
