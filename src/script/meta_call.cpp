/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "script/dispatcher.h"
#include "script/scripting.h"
#include "script/bind.h"
#include "script/lua.h"

#include "core/core.h"
#include "core/RTTI.h"
#include "go/go.h"

_CHAOS_BEGIN

// only for constructors, not for casting any more
// because first argument could be an object
// use literal casting function, say, cast
int	native_class_call( lua_State *L ){
	ScriptData *data = (ScriptData*)lua_touserdata(L, 1);

	ASSERT( data->dataType == ScriptData::Class );
	Type *type = data->class_->table->search( FACTORY_OP );

	if( type == 0 || type->type != Type::CONSTRUCTOR ){
		LOG("No constructor available, return none.");
		return 0;
	};

	LuaObjectFactory *factory = (LuaObjectFactory*)type->factory;
	void *obj = factory->createLuaObject(L);

	ScriptData *objData = (ScriptData*)lua_newuserdata( L, sizeof(ScriptData));
	objData->dataType = ScriptData::Instance;
	objData->type = data->class_;
	objData->obj = obj;

	native_instgc_meta( L );
	lua_setmetatable( L, -2 );

	return ScriptManager::getInstance()->return_(1);
}

int lua_statedef_call( lua_State* L){
	ScriptData *data = (ScriptData*)lua_touserdata( L, 1 );
	ASSERT( data != 0 && data->dataType == ScriptData::StateDef );

	luaL_argcheck( L, lua_gettop(L) == 2, 2, "Too many arguments." );
	luaL_argcheck( L, lua_istable(L,2), 1, "expected a state table." );

	lua_getfenv( L, 1 );
	lua_rawgeti( L, -1, 2);
	luaL_argcheck( L, lua_isnil( L, -1), 1, "already defined, duplicate calls.");

	lua_pushvalue(L, 2);
	lua_rawseti( L, -3, 2);

	lua_pushvalue( L, 1);
	return 1;
}

int lua_class_call(lua_State* L){
	ScriptData *data = (ScriptData*)lua_touserdata( L, 1 );
	ASSERT( data->dataType == ScriptData::GOClass && data->go_class != 0);
	GOClass* goclass = data->go_class;

	lua_remove(L, 1);	// remove the go class userdata

	GOInstance* go = goclass->construct(L);

	// create a shadow copy of instance
	data = (ScriptData*)lua_newuserdata(L, sizeof(ScriptData));
	data->dataType = ScriptData::Instance;
	data->type = go->getClassType();
	data->obj = go;

	// then assign the meta table to handle indexing
	lua_inst_puppet_meta( L);
	lua_setmetatable( L, -2);

	// retain the userdata
	//mLuaGO = TypeLua(lua_ref( L, 1));
	//lua_getref( L, go->getLuaGO().getRef() );		// return object as a user data
	return ScriptManager::getInstance()->return_(1);
}

_CHAOS_END