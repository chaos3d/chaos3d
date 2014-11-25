#include "script/type/convert.h"

using namespace script;

vector2f convert_from_lua<vector2f>::convert(lua_State* L, int idx) {
    luaL_argcheck(L, lua_istable(L, idx), idx, "expect a table");
    lua_rawgeti(L, idx, 1);
    lua_rawgeti(L, idx, 2);
    return vector2f(lua_tonumber(L, -2), lua_tonumber(L, -1));
}

void convert_to_lua<vector2f>::convert(lua_State* L, vector2f const& v) {
    lua_pushnumber(L, v.x());
    lua_pushnumber(L, v.y());
}

vector3f convert_from_lua<vector3f>::convert(lua_State* L, int idx) {
    luaL_argcheck(L, lua_istable(L, idx), idx, "expect a table");
    lua_rawgeti(L, idx, 1);
    lua_rawgeti(L, idx, 2);
    lua_rawgeti(L, idx, 3);
    return vector3f(lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1));
}

vector4f convert_from_lua<vector4f>::convert(lua_State* L, int idx) {
    luaL_argcheck(L, lua_istable(L, idx), idx, "expect a table");
    lua_rawgeti(L, idx, 1);
    lua_rawgeti(L, idx, 2);
    lua_rawgeti(L, idx, 3);
    lua_rawgeti(L, idx, 4);
    return vector4f(lua_tonumber(L, -4), lua_tonumber(L, -3),
                    lua_tonumber(L, -2), lua_tonumber(L, -1));
}

box2f convert_from_lua<box2f>::convert(lua_State* L, int idx) {
    luaL_argcheck(L, lua_istable(L, idx), idx, "expect a table");
    lua_rawgeti(L, idx, 1);
    lua_rawgeti(L, idx, 2);
    lua_rawgeti(L, idx, 3);
    lua_rawgeti(L, idx, 4);
    return box2f(vector2f(lua_tonumber(L, -4), lua_tonumber(L, -3)),
                 vector2f(lua_tonumber(L, -2), lua_tonumber(L, -1)));
}
