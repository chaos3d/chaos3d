/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "core/core.h"

#include "res/ScriptLoader.h"
#include "script/scripting.h"
#include "script/lua.h"
#include "script/dispatcher.h"

//using namespace chaos;

void ScriptResourceLoader::unload( Resource* res ){
	ScriptManager *sm = ScriptManager::getInstance();
	lua_State *L = sm->getState();
	lua_unref( L, mRefTable[res] );
	mRefTable.erase( res );
}

Resource* ScriptResourceLoader::load( DataStream* ds, char const* name ){
	ScriptManager* sm = ScriptManager::getInstance();
	ScriptThread* ss = sm->threadByData( ds, false );
	Resource* res = 0;
	
	if( !ss->run(true) ){	// yield?
		LOG("Yielded in the loading procedure, resource has not been fetched.");
	}

	if( ss != 0 && res != 0 ){
		lua_pushvalue( ss->getState(), 1 );
		res = (Resource*)ss->getValue<void>(-1);
		mRefTable[res] = lua_ref( ss->getState(), 1 );
	}

	return res;
}

///------------------------------------------------------------------
///------------------------------------------------------------------
void ScriptRefLoader::unload( Resource* res ){
	// did nothing
}

Resource* ScriptRefLoader::load( DataStream* ds, char const* name ){
	// did nothing, already loaded
	return 0;
}
	

///------------------------------------------------------------------
///------------------------------------------------------------------
void ScriptLoader::unload( Resource* res ){
	ScriptManager *sm = ScriptManager::getInstance();
	sm->releaseScript( (Script*)res );
}

Resource* ScriptLoader::load( DataStream* ds, char const* name ){
	ScriptManager *sm = ScriptManager::getInstance();

	return sm->scriptByData( ds );
}
	
