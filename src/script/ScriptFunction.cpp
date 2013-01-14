/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "script/ScriptManager.h"
#include "script/lua.h"
#include "script/ScriptData.h"
#include "script/dispatcher.h"

#include "go/go.h"

_CHAOS_BEGIN

void script_unfoldTable( lua_State *L, int idx ){
	// push itself if not a table
	if( lua_istable( L, idx ) == 0 ){
		lua_pushvalue( L, idx );
		return;
	}

	for(int n = luaL_getn( L, idx ), i = 1;i<=n;++i){
		lua_rawgeti( L, idx, i);
	}
}

// translate a 4byte char to integer
// typename('png')
int script_typename( lua_State *L ){
	int t = charsToTypename( luaL_checklstring( L, 1, 0 ) );
	lua_pushnumber( L, (lua_Number)t );
	return 1;
};

int	script_casttype( lua_State *L ){
	ScriptData* cdata = (ScriptData*)lua_touserdata(L,1);
	luaL_argcheck(L, cdata != 0 && cdata->dataType == ScriptData::Class, 1, "Expected a class definition");

	ScriptData* data = (ScriptData*)lua_touserdata(L,2);
	luaL_argcheck(L, data != 0 && data->dataType == ScriptData::Instance, 2, "Expected an object");

	data->type = cdata->class_;
	lua_pushvalue(L,2);
	return 1;
}
// register a resource to manager
// addResource( name, func );	-- a script creation resource
// addResource( name,  );	-- a script creation resource
int script_addResource( lua_State *L);

// define a new type of game object
/*
int script_defGO( lua_State *L ){
	char const* typeName = luaL_checklstring( L, 1, 0 );
	char const* parentName = 0;
	if( lua_type( L, 2 ) == LUA_TSTRING ){
		parentName = lua_tostring( L, 2 );
		luaL_checktype( L, 3, LUA_TTABLE);
	}else
		luaL_checktype( L, 2, LUA_TTABLE);

	ComponentDef cmpts[MAX_COMPONENTS];
	int noCompts = 0;

	lua_pushnil(L);
	while( lua_next( L, -2 ) != 0 && noCompts < MAX_COMPONENTS){
		char const*	cmptName = lua_tostring( L, -2 );

		ScriptData *lo = script::checkObject( L, -1 );
		Component*	cmpt = (Component*)script::getObject( L, -1 )->obj;
		cmpts[noCompts++] = ComponentDef( cmptName, cmpt );	

		lua_pop( L, 1);
	};

	if( noCompts == MAX_COMPONENTS)
		luaL_error( L, "Too many components for type (%s)", typeName);

	cmpts[noCompts] = ComponentDef( 0, 0);

	//GODatabase::getInstance()->addType( typeName, cmpts, parentName);

	return 0;
}*/

_CHAOS_END
