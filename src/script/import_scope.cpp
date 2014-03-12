#include "script/import_scope.h"
#include "script/state.h"

using namespace script;

import_scope::import_scope(char const* name, state& st)
: _state(&st) {
    lua_State* L = _state->internal();
    if (name == nullptr) {
        lua_pushvalue(L, LUA_GLOBALSINDEX);
    } else {
        lua_pushstring(L, name); // TODO: parse name
        lua_rawget(L, LUA_GLOBALSINDEX);
    }
    
    if (lua_isnoneornil(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_pushstring(L, name); // TODO: parse name
        lua_pushvalue(L, -2);
        lua_rawset(L, LUA_GLOBALSINDEX);
    }
}

lua_State* import_scope::get_L() const{
    return _state->internal();
}

import_scope& import_scope::def(const char *name, lua_function_t func) {
    lua_State* L = _state->internal();
    lua_pushstring(L, name);
    lua_pushcfunction(L, func);
    lua_rawset(L, -3);
    return *this;
}

import_scope& import_scope::save(char const* name) {
    lua_State* L = _state->internal();
    lua_pushstring(L, name);
    lua_insert(L, lua_gettop(L) - 1);
    lua_rawset(L, -3);
    return *this;
}

state& import_scope::end() {
    lua_pop(_state->internal(), 1);
    state& st = *_state;
    _state = nullptr;
    return st;
}