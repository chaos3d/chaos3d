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

#include "core/core.h"
#include "core/RTTI.h"
#include "go/go.h"

_CHAOS_BEGIN

int	native_class_index( lua_State *L ){
	// #userdata, key
	ScriptData *data = (ScriptData*)lua_touserdata(L, 1);
	ASSERT( data != 0 && data->dataType == ScriptData::Class );

	Type* type = data->type->table->search( lua_tostring(L,2) );
	if( type == 0 ){
		LOG("Uable to find the symbol (%s [%s]). return none. (%s)", lua_tostring(L,2), data->type->typeName(), ScriptManager::getInstance()->currentInfo().c_str());
		return 0;
	}

	if( type->type == Type::FUNCTION ){
		lua_pushnil(L);
		lua_pushlightuserdata(L, type->functor);
		lua_pushcclosure( L, type->functor->getFunc(), 2);

		// set the function reference
		// TODO: bug? forget why this
		lua_pushvalue(L,-1);
		lua_setupvalue(L, -1, 1);
		return 1;
	}else{
		LOG("FAILURE: Unsupported methods.");
		return 0;
	}
}

int native_inst_index( lua_State *L ){
	ScriptData *data = (ScriptData*)lua_touserdata(L, 1);
	ASSERT( data != 0 && data->dataType == ScriptData::Instance );

	if( data->type == 0 || data->type->type != Type::CLASS ){
		LOG("Unable to retreive the attributes because binding type is nil.");
		return 0;
	}

	Type* type = data->type->table->search( lua_tostring(L,2) );
	if( type == 0 ){
		LOG("Uable to find the symbol (%s [%s]). return none. (%s)", lua_tostring(L,2), data->type->typeName(), ScriptManager::getInstance()->currentInfo().c_str());
		return 0;
	}

	switch( type->type )
	{
	case Type::FUNCTION:		// return c closure
	{
		lua_pushvalue(L, 1);
		lua_pushlightuserdata(L, type->functor);
		lua_pushcclosure( L, type->functor->getFunc(), 2);
		return 1;
	}
	case Type::PROPERTY:		// getter
	{	if( type->getter == 0 ){
			LOG("Uable to find the symbol (%s [%s]). return none. (%s)", lua_tostring(L,2), data->type->typeName(), ScriptManager::getInstance()->currentInfo().c_str());
			return 0;
		}
		
		// the object is at the bottom and key is above
		lua_pushvalue( L, 1 );
		lua_pushlightuserdata(L, type->getter);
		lua_pushcclosure( L, type->getter->getFunc(), 2);
		lua_call( L, 0, LUA_MULTRET );
		return ScriptManager::getInstance()->return_(1);
	}
	default:
		LOG("FAILURE: Unsupported methods.");
		return 0;
		break;
	}
}

int	native_inst_newindex( lua_State *L ){
	ScriptData *data = (ScriptData*)lua_touserdata(L, 1);
	ASSERT( data != 0 && data->dataType == ScriptData::Instance );

	if( data->type == 0 || data->type->type != Type::CLASS ){
		LOG("Unable to set the attributes because binding type is nil.");
		return 0;
	}

	Type* type = data->type->table->search( lua_tostring(L,2) );
	if( type == 0 ){
		LOG("Uable to find the symbol (%s [%s]). return none. (%s)", lua_tostring(L,2), data->type->typeName(), ScriptManager::getInstance()->currentInfo().c_str());
		return 0;
	}

	if(type->type == Type::PROPERTY ){		// setter
		if( type->setter == 0 ){
			LOG("Unable to set the property (%s). return none.", lua_tostring(L,2));
			return 0;
		}
		
		// don't unfold the table,
		// we can set up a setter using a lua table directly
		lua_pushvalue( L, 1 );
		lua_pushlightuserdata(L, type->setter);
		lua_pushcclosure( L, type->setter->getFunc(), 2);

		lua_pushvalue( L, 3 ); // obj.set(val)
		lua_call( L, 1, LUA_MULTRET );
		return ScriptManager::getInstance()->return_(1);
	}else{
		LOG("Unable to assign new attributes to native type.");
		return 0;
	}
}

int lua_inst_index(lua_State* L){
#if 1
	ScriptData* data = (ScriptData*)lua_touserdata(L, 1);
	GOInstance* inst = (GOInstance*)data->obj;
	inst->getLuaVar().get(L);
	lua_pushvalue(L, 2);
	lua_rawget(L, -2);
	if( !lua_isnil(L, -1) )
		return 1;

	lua_pop(L, 2);
	lua_pushcclosure( L, lua_function_call, 2);
#else
	ScriptData* data = (ScriptData*)lua_touserdata(L, 1);
	GOInstance* inst = (GOInstance*)data->obj;
	ASSERT(data->dataType == ScriptData::Instance);

	ScriptManager *sm = ScriptManager::getInstance();
	TypeLua const& func( inst->getVar(lua_tostring( L, 2)) );

	lua_pushvalue(L,1);
	func.get(L);
	if( lua_isfunction(L,-1) == 1 && lua_iscfunction(L,-1) == 0 ){
		lua_pushcclosure( L, lua_function_call, 2);
	}
#endif
	return 1;
}

int lua_inst_newindex(lua_State* L){
	ScriptData* data = (ScriptData*)lua_touserdata(L, 1);
	GOInstance* inst = (GOInstance*)data->obj;
	ASSERT(data->dataType == ScriptData::Instance);

	TypeLua ss = inst->getLuaVar();
	ss.get(L);

	lua_insert(L, -3);		//# inst, table, key, value
	lua_rawset(L, -3);

	return 0;
}

_CHAOS_END