#include "script/LuaType.h"
#include "script/lua.h"

//using namespace chaos;

LuaType::LuaType(LuaType const& rhs) : _ref(-1){
    _host = rhs._host;
	if( rhs._ref != -1 ){
		lua_getref(_host, rhs._ref);
		_ref = lua_ref(_host, 1);
	}
}

LuaType::LuaType(ScriptState const& L, bool top) : _host(L), _ref(-1){
    if(top)
        _ref = lua_ref(L,1);
    else
        _ref = -1;
    _host = L;
}

LuaType& LuaType::operator=(LuaType const& rhs){
    ASSERT(rhs._host == _host);
	int old = _ref;
    lua_State* oL = _host.getState();
	if( rhs._ref != -1 ){
		lua_getref(rhs._host, rhs._ref);
		_ref = lua_ref(rhs._host, 1);
	}else
		_ref = -1;
	lua_unref(oL, old);
	return *this;
}

LuaType::~LuaType(){
	if(_ref >= 0){
		lua_unref(_host, _ref);
		_ref = -1;
	}
}

void LuaType::get(lua_State* l) const{
	ASSERT( l != 0 );
	lua_getref(l, _ref);
}

template<> void ScriptState::push_<LuaType>(lua_State *L, const LuaType &val){
    ASSERT(ScriptState(L) == _host);
    lua_getref(L, val.getRef());
}

template<> LuaType ScriptState::get_<LuaType>(lua_State *L, int idx){
    lua_pushvalue(L, idx);
    return LuaType(ScriptState(L));
}
