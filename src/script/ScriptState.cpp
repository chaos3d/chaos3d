/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "script/ScriptState.h"

#include "core/core.h"
#include "script/dispatcher.h"
#include "script/ScriptData.h"
#include "script/LuaType.h"
#include "script/ScriptManager.h"

//using namespace chaos;

///----------------------------------------------------------------
///----------------------------------------------------------------
template void* ScriptState::getValue<void>(int index) const;

ScriptState::ScriptState(){
	_L = lua_open();
	increseRef(1);
}

ScriptState::ScriptState(lua_State* L):_L(L){
	increseRef(1);
}

ScriptState::ScriptState(ScriptState const&ss):_L(ss._L){
	increseRef(1);
}

ScriptState& ScriptState::operator =(ScriptState const& rhs){
	_L == rhs._L;
	increseRef(1);
}

~ScriptState::ScriptState(){
	if(increseRef(-1) == 0)
		lua_close(_L);
}

int ScriptState::increseRef(int delta){
	lua_pushlightuserdata(_L, (void*)&ScriptState::ScriptState());
	lua_pushvalue(_L, -1);
	lua_rawget(_L, LUA_REGISTRYINDEX);
	int refc = lua_tonumber(_L, -1) + delta;
	lua_pop(L, 1);

	lua_pushnumber(_L, refc);
	lua_rawset(_L, LUA_REGISTRYINDEX);
}

void* ScriptState::getObject(int index) const{
	if( lua_isnoneornil( mL, index))
		return 0;

	ScriptData *data = (ScriptData*)lua_touserdata(mL,index);
#if defined(DEBUG)
	if( data == 0 ){
		LOG( "Not a userdata type at index %d (%s)", index, ScriptManager::getInstance()->currentInfo().c_str());
	}else if( data->dataType != ScriptData::Instance )
		LOG( "Not an instance type at index %d (%s)", index, ScriptManager::getInstance()->currentInfo().c_str());
	else
#endif
		if( data != 0 )
		return data->obj;
	return 0;
}

static const char *streamReader (lua_State *L, void *data, size_t *size){
	static char buffer[1024];
	SourceReader* ds = (SourceReader*)data;

	*size = ds->read(buffer, 1024);
	return buffer;
}

template<> 
void ScriptState::push_<SourceReader*>(SourceReader* sr){
	lua_load(mL, streamReader, (void*)sr, sr->source());
	// todo: error check
}

/*
template<>
ReferencedCount* const& chaos::ScriptState::getValue<ReferencedCount>(int index){
	return *(ReferencedCount*const*)&getValue<void>(index);
}
*/
template<>
void ScriptState::pushValue<void*>(void* obj, Type* type, bool gc) const{
	if( obj == 0 ){
		lua_pushnil(mL);
		return;
	};

	ScriptData *data = (ScriptData*)lua_newuserdata(mL, sizeof(ScriptData));
	data->dataType = ScriptData::Instance;
	data->type = type;
	data->obj = obj;

	if( gc )
		native_instgc_meta( mL );
	else
		native_inst_meta( mL );

	lua_setmetatable( mL, -2 );
}

void ScriptState::pushObject(void* obj, Type* type, bool gc) const{
	if( obj == 0 ){
		lua_pushnil(mL);
		return;
	};

	ScriptData *data = (ScriptData*)lua_newuserdata(mL, sizeof(ScriptData));
	data->dataType = ScriptData::Instance;
	data->type = type;
	data->obj = obj;

	/*if( type != 0 && type->isDerived(TYPE(ReferencedCount)) ){
		ReferencedCount* obj((ReferencedCount*)data->obj);
		data->type = obj->getClassType();
		data->obj = obj;
		obj->retain();

		native_instref_meta( mL );
	}else*/ if( gc )
		native_instgc_meta( mL );
	else
		native_inst_meta( mL );

	lua_setmetatable( mL, -2 );
}

// ref count pointer as a native type
template<>
ReferencedPtr ScriptState::getNativeValue<ReferencedPtr>(int index) const{
	//static void* zero = 0;
	ScriptData *data = (ScriptData*)lua_touserdata(mL,index);
	if( data == 0 ){
		LOG( "Not a userdata type at index %d", index);
	}else if( data->dataType != ScriptData::Instance )
		LOG( "Not an instance type at index %d", index);
	else
		return ReferencedPtr((ReferencedCount*)data->obj);	//todo: type checking, ref count
	return ReferencedPtr(0);
}

template<>
void ScriptState::pushValue<ReferencedCount*>(ReferencedCount* obj, Type* type, bool gc) const{
	if( obj == 0 ){
		lua_pushnil(mL);
	}else{
		ScriptData *data = (ScriptData*)lua_newuserdata(mL, sizeof(ScriptData));
		data->dataType = ScriptData::Instance;
		data->type = type;//obj->getClassType();
		data->obj = obj;
		obj->retain();

		native_instref_meta( mL );
		lua_setmetatable( mL, -2 );
	}
}
