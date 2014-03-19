#include "script/import_scope.h"
#include "script/state.h"

using namespace script;

import_scope::import_scope(char const* name, state& st)
: _state(&st) {
    lua_State* L = _state->internal();
    lua_pushvalue(L, LUA_GLOBALSINDEX);

    if (name != nullptr) {
        char const* dot = nullptr;
        do {
            dot = strchr(name, '.');
            if (dot == nullptr) {
                dot = name + strlen(name);
            }
            if (dot != name) {
                lua_pushlstring(L, name, dot - name);       // #_G, name
                lua_rawget(L, -2);                          // #_G, tbl
                if (lua_isnil(L, -1)) {                     // #_G, nil
                    lua_pop(L, 1);                          // #_G
                    lua_newtable(L);                        // #_G, tbl
                    lua_pushlstring(L, name, dot - name);   // #_G, tbl, key
                    lua_pushvalue(L, -2);                   // #_G, tbl, key, tbl
                    lua_rawset(L, -4);                      // #_G, tbl
                }
                lua_remove(L, -2);                          // #tbl
            }
            name = dot + 1;
        } while (*dot);
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

template<>
import_scope& import_scope::import<std::type_info const*>(char const* name,
                                                          std::type_info const* value) {
    lua_pushlightuserdata(_state->internal(), (void*)value);
    return save(name);
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