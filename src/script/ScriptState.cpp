/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "ScriptState.h"
#include <cstring>

//using namespace chaos;

///----------------------------------------------------------------
///----------------------------------------------------------------
//template void* ScriptState::getValue<void>(int index) const;

ScriptState::ScriptState(){
	_L = lua_open();
	lua_pushlightuserdata(_L, (void*)signature());
	lua_pushthread(_L);
	lua_rawset(_L, LUA_REGISTRYINDEX);
	increseRef(1);
}

ScriptState::ScriptState(lua_State* L):_L(L){
	increseRef(1);
}

ScriptState::ScriptState(ScriptState const&ss):_L(ss._L){
	increseRef(1);
}

ScriptState& ScriptState::operator=(ScriptState const& rhs){
	if(*this == rhs)
		_L = rhs._L;
	else{
		increseRef(-1);
		_L = rhs._L;
		increseRef(1);
	}
    return *this;
}

bool ScriptState::operator==(ScriptState const& rhs) const{
	return signature() == rhs.signature();
};

bool ScriptState::operator!=(ScriptState const& rhs) const{
	return signature() != rhs.signature();
};

ScriptState::~ScriptState(){
	if(increseRef(-1) == 0){
		lua_pushlightuserdata(_L, (void*)signature());
		lua_rawget(_L, LUA_REGISTRYINDEX);
		lua_State *mainL = lua_tothread(_L, -1);
		lua_close(mainL);
	}
}

const void* ScriptState::signature() const {
	lua_pushvalue(_L, LUA_GLOBALSINDEX);
	const void* s = lua_topointer(_L, -1);
	lua_pop(_L, 1);
	return s;
}

void ScriptState::pushUD(void* obj, void* meta){
    void** ptr = (void**)lua_newuserdata(_L, sizeof(void*));
    *ptr = obj;
    
    pushMetatable(meta);
    lua_setmetatable(_L, -2);
}

void ScriptState::pushMetatable(void*){
    
}

void ScriptState::pop(int n) {
    lua_pop(_L, n);
}

int ScriptState::top() const {
    return lua_gettop(_L);
}

int ScriptState::increseRef(int delta){
    static int _increase_sig = 0;
	lua_pushlightuserdata(_L, (void*)&_increase_sig);
	lua_pushvalue(_L, -1);
	lua_rawget(_L, LUA_REGISTRYINDEX);
	int refc = lua_tonumber(_L, -1) + delta;
	lua_pop(_L, 1);

	lua_pushnumber(_L, refc);
	lua_rawset(_L, LUA_REGISTRYINDEX);
    return refc;
}

static const char *streamReader (lua_State *L, void *data, size_t *size){
	static char buffer[1024];
    ScriptState::SourceReader* ds = (ScriptState::SourceReader*)data;

	*size = ds->read(buffer, 1024);
	return buffer;
}

ScriptState::StringReader::StringReader(char const* str):_strbuf(str), _curpos(0){
    _bufsize = strlen(str);
}

size_t ScriptState::StringReader::read(char *buf, size_t buf_size){
    if(_bufsize - _curpos < buf_size)
        buf_size = _bufsize - _curpos;
    memcpy(buf, _strbuf + _curpos, buf_size);
    _curpos += buf_size;
    return buf_size;
}

template<> 
void ScriptState::push_<ScriptState::SourceReader>(lua_State * L, ScriptState::SourceReader * sr){
	lua_load(L, streamReader, (void*)sr, sr->source());
	// todo: error check
}

template<> bool ScriptState::get_<bool>(lua_State *L, int idx){
    return lua_toboolean(L, idx) == 1;
}

template<> void ScriptState::push_<bool>(lua_State *L, bool v){
    lua_pushboolean(L, v ? 1 : 0);
}

