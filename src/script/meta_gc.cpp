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

int lua_inst_gc(lua_State* L){
	ScriptData *data = (ScriptData*)lua_touserdata( L, 1 );
	ASSERT( data != 0 && data->dataType == ScriptData::Instance );

	GOInstance* goinst = (GOInstance*)data->obj;
	data->go_type->destruct( goinst );

	return ScriptManager::getInstance()->return_(0);
}

int lua_state_gc(lua_State* L){
	ScriptData *data = (ScriptData*)lua_touserdata( L, 1 );
	ASSERT( data != 0 && data->dataType == ScriptData::State );

	delete data->state;
	return ScriptManager::getInstance()->return_(0);
}

int	native_inst_gc( lua_State *L ){
	ScriptData *data = (ScriptData*)lua_touserdata(L, 1);
	ASSERT( data != 0 && data->dataType == ScriptData::Instance );
	Type *type = data->type->table->search( FACTORY_OP );

	ASSERT( type != 0 && type->type == Type::CONSTRUCTOR );

	((LuaObjectFactory*)type->factory)->destroyLuaObject( data->obj );
	return ScriptManager::getInstance()->return_(0);
}

int	native_inst_ref_gc( lua_State *L ){
	ScriptData *data = (ScriptData*)lua_touserdata(L, 1);
	ASSERT( data != 0 && data->dataType == ScriptData::Instance );

	((ReferencedCount*)data->obj)->release();

	return ScriptManager::getInstance()->return_(0);
}

int lua_meta_tostring( lua_State* L){
	ScriptData *data = (ScriptData*)lua_touserdata(L, 1);
	ASSERT( data != 0 );

	lua_pushfstring( L, "ADDRESS: %p, TYPE: %s", data, ScriptData::typeName(data->dataType) );

	switch( data->dataType )
	{
	case ScriptData::Instance:
		lua_pushfstring( L, " [Class: %s, Obj: %p]", data->type ? data->type->typeName() : "nil", data->obj );
		break;
	case ScriptData::Class:
		lua_pushfstring( L, " [Class Definition]\n%s", data->class_->description().c_str() );
		break;
	default:
		lua_pushstring(L, "");
	};

	lua_concat(L,2);
	return 1;//ScriptManager::getInstance()->return_(1);
}

_CHAOS_END
