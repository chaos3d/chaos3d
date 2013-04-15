#ifndef _WRAPPER_LUA_H
#define _WRAPPER_LUA_H

class DataStream;
class DataSource;

extern int luaio_load(lua_State *, DataStream* , char const* )
extern int luaio_dump(lua_State *, DataSource* )

#endif
