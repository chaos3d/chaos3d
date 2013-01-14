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
//#include "go/go.h"

//using namespace chaos;

void ScriptManager::loadSymbol(const char *name, void* obj, Type* type){
	lua_pushvalue(mL, LUA_GLOBALSINDEX);
	lua_pushstring(mL, name);

	ScriptData* data = (ScriptData*)lua_newuserdata(mL, sizeof(ScriptData) );
	data->dataType = ScriptData::Instance;
	data->type = type;
	data->obj = obj;

	native_inst_meta(mL);
	lua_setmetatable(mL, -2);

	lua_rawset(mL, -3);
	lua_pop(mL,1);
}

void ScriptManager::loadSymbols(const char *name, SymbolTable *table){

	if( table == 0 )
		table = GSymbolTable::getInstance();

	if( table != 0 ){
		if( name != 0 ){
			lua_newtable(mL);
			lua_setglobal( mL, name);
		}else if(! lua_istable(mL, -1 ) )
			name = 0;
	}

	if( name != 0 )
		lua_getglobal( mL, name);
	else 
		lua_pushvalue( mL, LUA_GLOBALSINDEX);


	// copy names into table
	for(SymbolTable::TTabMap::const_iterator it = table->getTable().begin();
		it != table->getTable().end();
		++it)
	{
		Type* type = it->second;
		if( type->type == Type::NAMESPACE ){
			if( *it->first != '\0' ){
				lua_pushstring( mL, it->first);
				lua_newtable( mL );
			}
			loadSymbols( 0, type->table);
			if( *it->first != '\0' ){
				lua_rawset( mL, -3 );
			}
		}else if( type->type == Type::STATIC ){
			lua_pushstring( mL, it->first );

			ScriptData* data = (ScriptData*)lua_newuserdata(mL, sizeof(ScriptData) );
			data->dataType = ScriptData::Instance;
			data->type = type->objType;
			data->obj = type->obj;

			native_inst_meta( mL );
			lua_setmetatable( mL, -2 );

			lua_rawset( mL, -3 );
		}else if( type->type == Type::CLASS ){
			ASSERT( *it->first != '\0' );
			lua_pushstring( mL, it->first);

			ScriptData* data = (ScriptData*)lua_newuserdata(mL, sizeof(ScriptData) );
			data->dataType = ScriptData::Class;
			data->class_ = type;

			native_class_meta( mL );
			lua_setmetatable( mL, -2 );

			lua_rawset( mL, -3 );
		}else if( type->type == Type::FUNCTION ){
			ASSERT( *it->first != '\0');
			lua_pushstring( mL, it->first);	// push name
			lua_pushnil(mL);	// conform to native call, upvalue(1) == obj
			
			ScriptData* data = (ScriptData*)lua_newuserdata(mL, sizeof(ScriptData) );
			data->dataType = ScriptData::Class;
			data->class_ = type;

			lua_pushcclosure( mL, native_function_call, 2);

			lua_rawset( mL, -3 );
		}
	}

	lua_pop(mL,1);
}
