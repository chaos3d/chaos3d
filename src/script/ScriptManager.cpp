/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "script/ScriptManager.h"

#include "core/core.h"
#include "io/io.h"
#include "anim/AnimationTicker.h"
#include "core/SequenceCreator.h"
#include "core/Timer.h"
#include "core/Sequence.h"
#include "script/ScriptThread.h"
#include "script/dispatcher.h"
#include "script/Script.h"
#include "script/bind.h"
#include "script/lua.h"

//using namespace chaos;
using namespace std;

IMPLEMENT_CLASS(ScriptManager, LIB_DOMAIN )
//IMPLEMENT_FUNC( run, (ScriptState* (ScriptManager::*)(Script*))&ScriptManager::schedule)
//IMPLEMENT_FUNC( justRun, (ScriptState* (ScriptManager::*)(Script*))&ScriptManager::schedule)
IMPLEMENT_FUNC( run, (ScriptThread* (ScriptManager::*)(char const*))&ScriptManager::schedule)
IMPLEMENT_FUNC( load, &ScriptManager::threadByData)
IMPLEMENT_FUNC( current, &ScriptManager::current)
IMPLEMENT_FUNC( dispose, &ScriptManager::dispose)
IMPLEMENT_FUNC( timer, (void (ScriptManager::*)(float))&ScriptManager::timer)
IMPLEMENT_PROP( updatingScript, &ScriptManager::setUpdatingScript, &ScriptManager::getUpdatingScript)
IMPLEMENT_END;

_CHAOS_BEGIN
extern int script_typename(lua_State* L);
extern int script_casttype(lua_State* L);
extern int script_defGO( lua_State *L );
extern int script_shuffle( lua_State *L );
_CHAOS_END

/// ----------------------------------------------
static const char *streamReader (lua_State *L, void *data, size_t *size){
	static char buffer[1024];
	DataStream* ds = (DataStream*)data;

	*size = ds->read(buffer, 1024);
	return buffer;
}

static int sourceWriter (lua_State *L, const void* p, size_t sz, void* ud){
	DataSource *ds = (DataSource*)ud;
	if( ds->write( p, sz ) == sz )
		return 0;
	else
		return -1;
}

static void luaHook(lua_State* l, lua_Debug *ar){
	ScriptManager::Debugger& debugger(*ScriptManager::getInstance()->getDebugger());

	switch( ar->event )
	{
	case LUA_HOOKCALL:
		return debugger.call(l, ar);
	case LUA_HOOKRET:
	case LUA_HOOKTAILRET:
		return debugger.return_(l, ar);
	case LUA_HOOKLINE:
		return debugger.line(l, ar);
	case LUA_HOOKCOUNT:
		return debugger.count(l, ar);
	}
}

// TODO: only asset is needed
// separate directly loading by another loader to surpress warnings
#define LIB_SEARCHURL "doc://?.lc;doc://?.lua;app://?.lua;app://?.lc;asset://?.lc;asset://?.lua"

static const char *pushnexttemplate(lua_State *L, const char *path)
{
	const char *l;
	while (*path == *LUA_PATHSEP) path++;  /* skip separators */
	if (*path == '\0') return NULL;  /* no more templates */
	l = strchr(path, *LUA_PATHSEP);  /* find next separator */
	if (l == NULL) l = path + strlen(path);
	lua_pushlstring(L, path, (size_t)(l - path));  /* template */
	return l;
}

static DataStream *searchpath (lua_State *L, const char *name,
							   const char *url)
{
	DataStream* ds = 0;
	while ((url = pushnexttemplate(L, url)) != NULL) {
		const char *filename = luaL_gsub(L, lua_tostring(L, -1),
										 LUA_PATH_MARK, name);
		ds = IOManager::getInstance()->streamByURL(filename);
		lua_pop(L, 2);	// remove template and filename after using it
		if(ds != 0 && ds->valid())
			break;
	}
	return ds;
}

static int lua_streamLoader(lua_State* L){
	char const* fileName = lua_tostring(L, 1);
	DataStream *ds = searchpath(L, fileName, LIB_SEARCHURL);

	if(ds != 0)
		lua_load(L, streamReader, (void*)ds, fileName);
	
	return 1;
};

/// ----------------------------------------------
/// ----------------------------------------------


/// ----------------------------------------------
/// ----------------------------------------------
int ScriptManager::fork(lua_State* L){
	ScriptManager* sm = ScriptManager::getInstance();
	
	//lua_getglobal( L, "this");
	//int this_ = lua_ref(L, 1);

	for( int i=lua_gettop(L);i>0;--i){
		if( !lua_isfunction(L,-1) ){
			LOG("Unable to fork not a function. ignored");
			lua_pop( L, 1);
			continue;
		}

		// add thread to next run
		ScriptThread* ss = ScriptThread::thread(TypeLua(L));
		ss->run( false );
	}

//	lua_unref(L, this_);

	// yield to next run
	ScriptThread::current()->wait4( );

	return sm->return_(0);
}

/// ----------------------------------------------
/**
 * pulse events
 */
static int _pulse(lua_State* L){
	for( int i = lua_gettop(L); i>0 ; --i){
		ScriptData *sd = (ScriptData*) lua_touserdata(L, i);
		if( sd != 0 && sd->obj != 0 )
			Sequence::getInstance()->pulse( sd->obj );
	}	
	return ScriptManager::getInstance()->return_(0);
}

/**
 * create a thread using a function
 * which can be used in a sequence
 */
static int _thread(lua_State* L){
	luaL_checktype( L, 1, LUA_TFUNCTION );
	
	ScriptState state( L );
	state.push_( ScriptThread::thread(TypeLua(L)) );
	
	return 1;
}

/**
 * create a ticker triggerable
 */
static int _tick(lua_State* L){
	
	// TODO: tmp code
	if( lua_gettop(L) == 0 ){
		lua_pushnumber(L, (int)(AnimationState::getGlobal()*30.f)); // TODO Application::getInstance()->getDesiredFps()));
		return 1;
	}else{
		ScriptManager* sm = ScriptManager::getInstance();
		int tick = (int)luaL_checknumber(L, 1);
		if( tick <= 1 ) {	// yield
			sm->current()->wait4(&Triggerable::sDeferedSignal);
		} else {
			sm->timer(tick/30.f); // TODO Application::getInstance()->getDesiredFps() );			
		}
		return sm->return_(0);
		//return lua_yield(L,0);
	}
}

/**
 * start to declare a sequence
 */
static int _event(lua_State* L){
	luaL_checktype(L, 1, LUA_TUSERDATA);

	ScriptData *sd = (ScriptData*)lua_touserdata(L, 1);
	luaL_argcheck( L, sd->dataType == ScriptData::Instance && sd->type->isDerived( TYPE_FROM_NAME(Triggerable) ), 1, "expected a triggerable" );
	Triggerable *t((Triggerable*)sd->obj);

	SequenceCreator *sc = new SequenceCreator( t );
	ScriptState state( L );
	state.push_( sc );
	sc->release();
	return 1;
}


/// ----------------------------------------------
int ScriptManager::url(lua_State* L){
	ScriptManager* sm = ScriptManager::getInstance();
	IOManager* io = IOManager::getInstance();
	
	lua_pushstring( L, io->nameByURL(luaL_checkstring(L,1)).c_str() );
	
	return sm->return_(1);
}

/// ----------------------------------------------
int ScriptManager::timer(lua_State* L){
	ScriptManager* sm = ScriptManager::getInstance();
	
	if( lua_gettop(L) == 0 ){
		lua_pushnumber(L, AnimationState::getGlobal());
		return 1;
	}else{
		float tick = (float)luaL_checknumber(L, 1);
		sm->timer( tick );
		return sm->return_(0);
	}
}

int ScriptManager::swait(lua_State* L){
	ScriptManager* sm = ScriptManager::getInstance();
	ScriptThread* st = sm->current();
	int n = lua_gettop(L);
	bool any = (lua_isboolean(L,n) && lua_toboolean(L,n) == 1);
	if(any)
		st->setAny(true), n-=1;
	for(int i=1;i<=n;++i){
		ScriptData* data = (ScriptData*)lua_touserdata(L,i);
		
		if( data == 0 || data->dataType != ScriptData::Instance)
			LOG("Uable to wait (%s)", luaL_typename(L,i));
		else{
			st->wait4( (ScriptThread*)data->obj );
		}
	}
	return sm->return_(0);
}

void ScriptManager::timer( float t ){
	AnimationTicker *ticker = new AnimationTicker();
	ticker->play(t, 1.f, -1.f );	// will fail at the start
	ticker->release();

	ScriptThread::current()->wait4( ticker );
}
/// ----------------------------------------------
ScriptManager::ScriptManager(Timer const& t, Sequence const& s)
	: mRunning(), mSymbolTbl(new GSymbolTable()),
	mDebugger(0), mTimer(t), mSequence(s)
{
	mRunning.push_back(0);		// push the marker
	//mThreads[0].clear();

	memset( &stat, 0, sizeof( stat ));
	static const luaL_Reg regs[] = {
		"url", url,
		"cast", script_casttype,
		"shuffle", script_shuffle,
		"fork", ScriptManager::fork,
		"timer", ScriptManager::timer,
		"wait", ScriptManager::swait,
		"typename", script_typename,
//		"def", script_defGO,
		"class", script_class,
		"state", script_state,
		"thread", _thread,
		"tick", _tick,
		"event", _event,
		"pulse", _pulse,
		0, 0,
	};

	mL = lua_open();
	luaL_openlibs( mL );

	// set stream loader
	lua_getglobal( mL, "package");
	lua_getfield( mL, -1, "loaders");
	for(int i=luaL_getn( mL, -1); i>1;--i ){
		lua_rawgeti(mL, -1, i);
		lua_rawseti(mL, -2, i+1);	// package.loaders[i+1] = package.loaders[i]
	}
	lua_pushcfunction( mL, lua_streamLoader);
	lua_rawseti( mL, -2, 2); // package.loaders[2] = streamLoader

	lua_getglobal( mL, "_G" );
	luaL_register( mL, 0, regs );
	lua_pop(mL, 1);
}
/// ----------------------------------------------

/// ----------------------------------------------
ScriptManager::~ScriptManager(){
	for( TThPool::iterator it = mThPool.begin(); it != mThPool.end(); ++it){
		//lua_unref( mL, (*it)->mRef);
		(*it)->mStatus = ScriptThread::None;
		(*it)->release();
	}

	LOG("Total No. of threads: %d", stat.total);
	LOG("No. of newly created threads: %d", stat.totalCreated);
	LOG("No. of threads created from pool: %d", stat.numPool);
	LOG("No. of threads for pool (normal ternimated): %d", stat.numForPool);
	LOG("No. of threads not for pool (abnormal terminated): %d", stat.numNotForPool);
	LOG("Max No. of pooled threads (size of pool): %d", stat.maxPool);
	LOG("Max No. of running(yielding) threads: %d", stat.maxRun);
	lua_close( mL );
	delete mSymbolTbl;
}
/// ----------------------------------------------

/// ----------------------------------------------
ScriptThread* ScriptManager::schedule( char const* url ){
	return threadByData(IOManager::getInstance()->streamByURL( url ), true );
}


Script* ScriptManager::scriptByData( DataStream* ds ){
	if( ds == 0 ){
		LOG("Script not loaded.");
		return 0;
	}else 	if( lua_load( mL, streamReader, (void*)ds, ds->where() ) != 0 ){
		LOG("Script compiling errors: %s", lua_tostring(mL, -1));
		lua_settop(mL, 0);
		return 0;
	}

	return new Script(lua_ref( mL,1 ));
}

ScriptThread* ScriptManager::threadByData( DataStream* ds, bool scheduled ){
	if( ds == 0 ){
		LOG("Script not loaded.");
		return 0;
	}else if( lua_load( mL, streamReader, (void*)ds, ds->where() ) != 0 ){
		LOG("Script compiling errors: %s", lua_tostring(mL, -1));
		lua_settop(mL, 0);
		return 0;
	}

	ScriptThread* ss = ScriptThread::thread( TypeLua(mL) );
	if( scheduled )
		ss->run( false );

	return ss;
}
/// ----------------------------------------------

/// ----------------------------------------------
std::string	ScriptManager::currentInfo(){
	ScriptThread *ss(mRunning.back());
	lua_State *L(mL);
	if( ss != 0 )
		L = ss->getState();

	luaL_where(L, 1);
	string info(lua_tostring(L,-1));
	lua_pop(L,1);
	return info;
}

void ScriptManager::enableDebug(Debugger *d){
	if( mDebugger != 0 )
		delete mDebugger;

	mDebugger = d;
	if( d == 0 )
		lua_sethook(mL, 0, 0, 0);
	else
		lua_sethook(mL, luaHook, LUA_MASKCALL|LUA_MASKRET|LUA_MASKLINE|LUA_MASKCOUNT, 1);
}
/// ----------------------------------------------

/// ----------------------------------------------
bool ScriptManager::dump( int ref, DataSource* ds ){
	if( ds == 0 )
		return false;

	int ret = -1;
	lua_getref( mL, ref );
	if( lua_isfunction( mL, -1 ) ){
		ret = lua_dump( mL, sourceWriter, ds );
	}

	lua_pop(mL,1);
	return ret == 0;
}

bool ScriptManager::dumpScript( Script* script, DataSource* ds ){
	return dump( script->mRef, ds );
}
/// ----------------------------------------------

static int traceback(lua_State *L)
{
	if (!lua_isstring(L, 1))  /* 'message' not a string? */
		return 1;  /* keep it intact */
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		return 1;
	}
	lua_getfield(L, -1, "traceback");
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 2);
		return 1;
	}
	lua_pushvalue(L, 1);  /* pass error message */
	lua_pushinteger(L, 2);  /* skip this function and traceback */
	lua_call(L, 2, 1);  /* call debug.traceback */
  return 1;
}

/// ----------------------------------------------
bool ScriptManager::bootstrap(DataStream* ds){
	threadByData(ds, false)->run(true);
	//lua_State* L = lua_newthread(mL);
//	lua_State* L = mL;
//	int base = lua_gettop(mL);
//	lua_pushcfunction(L, traceback);
//	lua_getglobal(L, "require");
//	lua_pushstring(L, "bootstrap");
//	int ret = lua_pcall(mL, 1, 0, base);//(L, 1);
//	if(ret == LUA_YIELD){
//		lua_pushstring(L, "resume");
//		lua_resume(L, 1);
//	}else {
//		lua_getfield(mL, LUA_GLOBALSINDEX, "debug");
//		lua_getfield(mL, -1, "traceback");
//		lua_pushthread(L);
//		//lua_pushvalue(L, 1);  /* pass error message */
//		//lua_pushinteger(L, 2);  /* skip this function and traceback */
//		lua_pcall(L, 1, 1, 0);  /* call debug.traceback */
//		LOG("Bootstrap running errors: %s", luaL_typename(mL, -1));		
//	}

	/*if(lua_resume(L, 1)!=0){
		LOG("Bootstrap running errors: %s", lua_tostring(mL, -1));
		return false;
	}*/
	/*
	if( ds == 0 || !ds->valid() ){
		LOG("Unable to run the bootstrap.");
		return false;
	}

	if( lua_load( mL, streamReader, (void*)ds, "bootstrap" ) != 0 ){
		LOG("Unable to load the bootstrap: %s", lua_tostring(mL, -1));
		return false;
	}

	// may consider default bootstrap parameters
	if( lua_pcall( mL, 0, 0, 0) != 0 ){
		LOG("Bootstrap running errors: %s", lua_tostring(mL, -1));
		return false;
	}

	if( lua_status( mL ) == LUA_YIELD ){
		LOG("Bootstrap script cannot be yielded.");
		return false;
	}*/

	return true;
}
/// ----------------------------------------------

/// ----------------------------------------------
void ScriptManager::releaseScript( Script* script){
	if( script == 0 )
		return;

	lua_unref( mL, script->mRef);
	delete script;
}

/// ----------------------------------------------

/// ----------------------------------------------
bool ScriptManager::run(ScriptThread* ss){
	if( ss == 0 || ss->getStatus() == ScriptThread::Terminated)
		return true;

	int status(ss->mStatus);
	// not resume c func, go dispose it
	if( status == ScriptThread::CYielded ){
		ss->dispose();
		-- stat.numCurRun;
		return true;
	}
	
	mRunning.push_back(ss);

	lua_State *L = ss->getState();
	lua_getglobal( mL, "this" );
	TypeLua savedThis(mL);

	lua_getref(mL, ss->mThis.getRef() );
	lua_setglobal( mL, "this");
	
	ss->mStatus = ScriptThread::Running;
	++ stat.numCurRun;
	
	int ret(0);
	int top(lua_gettop(L));
	bool cfunc(false);
	if( status == ScriptThread::Yielded ){	
		ret = lua_resume( L, top );
	}else if(status == ScriptThread::Created ){
//		int base = lua_gettop(L);
		cfunc = lua_iscfunction(L, 1) == 1;
//		lua_pushcfunction(L, traceback);
//		lua_insert(L, base);
//		ret = lua_pcall(L, top-1, 0, base);//(L, 1);
		ret = lua_resume(L, top - 1);//, 0, 0);
	}else {	// something wrong here
		ASSERT("Thread status not correctly set");
	}

	savedThis.get(mL);
	lua_setglobal( mL, "this");

	mRunning.pop_back();

	if( ret == LUA_YIELD ){
		ss->mStatus = cfunc ? ScriptThread::CYielded : ScriptThread::Yielded;
		stat.maxRun = stat.numCurRun > stat.maxRun ? stat.numCurRun : stat.maxRun;
	}else{
		if( ret != 0 ){
			lua_getfield(mL, LUA_GLOBALSINDEX, "debug");
			lua_getfield(mL, -1, "traceback");
			lua_pushthread(L);
			lua_xmove(L, mL, 1);
			lua_xmove(L, mL, 1);
			//lua_pushvalue(L, 1);  /* pass error message */
			//lua_pushinteger(L, 2);  /* skip this function and traceback */
			lua_call(mL, 2, 1);  /* call debug.traceback */
			LOG( "Script running errors: %s.", lua_tostring(mL,-1));
		}

		ss->dispose();
		-- stat.numCurRun;
	}
	return ret != LUA_YIELD;
}
/// ----------------------------------------------

void ScriptManager::dispose(ScriptThread* ss){
	if( ss != 0 )
		ss->dispose();
}

void ScriptManager::recycleThread( ScriptThread* ss){
	// we can only pool the normal status thread
	if( lua_status( ss->mL ) == 0 ){
		// keep the varibles for a while;
		//lua_settop(ss->mL, 0);

		mThPool.push_back( ss );

		++stat.numForPool;
		stat.maxPool = stat.maxPool > (int)mThPool.size() ? stat.maxPool : mThPool.size();
	}else{
		ss->release();
		++stat.numNotForPool;
	}
}

ScriptThread* ScriptManager::createThread(){
	ScriptThread* ss(0);

	if( mThPool.empty() ){
		lua_State* L = lua_newthread( mL );
		ss = new ScriptThread(L, TypeLua( mL ));
		++stat.totalCreated;
	}else{
		++ stat.numPool;
		ss = mThPool.back();
		mThPool.pop_back();
	}

	ss->mStatus = ScriptThread::Created;
	lua_settop(ss->getState(),0);

	++ stat.total;
	return ss;
}

/// ----------------------------------------------
int ScriptManager::return_(int args){
	if(mRunning.back() != 0 && mRunning.back()->isSuspended() ){
		return lua_yield( mRunning.back()->mL, 0 );	
		// when yields, return value doesn't make sense until the point the event is triggered
	}else
		return args;
}

bool ScriptManager::runScripts(){

	// run updating loop
	if( mUpdatingScript.getRef() != -1 ){
		mUpdatingScript.get( mL );

		// may consider other parameters
		if( lua_pcall( mL, 0, 0, 0) != 0 ){
			LOG("Running errors@updating loop: %s", lua_tostring(mL, -1));
		}
		
		if( lua_status( mL ) == LUA_YIELD ){
			LOG("Updating script cannot be yielded.");
		}
	}
	
#ifdef DEBUG
	lua_gc( mL, LUA_GCCOLLECT, 0 );
#else
	lua_gc( mL, LUA_GCSTEP, 2 );
#endif

	return true;
}
/// ----------------------------------------------
