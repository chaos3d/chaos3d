#include "script/LuaType.h"
#include "script/ScriptManager.h"
#include "script/lua.h"

//using namespace chaos;

TypeLua::TypeLua(TypeLua const& rhs) : ref(-1){
	if( rhs.ref != -1 ){
		lua_State *L(ScriptManager::getInstance()->getState());
		lua_getref(L, rhs.ref);
		ref = lua_ref(L, 1);
	}
}

TypeLua::TypeLua(lua_State* L) : ref(-1){
	//if(!lua_isnil(L,-1))
	ref = lua_ref(L,1);
}

TypeLua& TypeLua::operator=(TypeLua const& rhs){
	lua_State *L(ScriptManager::getInstance()->getState());
	int old = ref;
	if( rhs.ref != -1 ){
		lua_getref(L, rhs.ref);
		ref = lua_ref(L, 1);
	}else
		ref = -1;
	lua_unref(L, old);
	return *this;
}

TypeLua::~TypeLua(){
	if( ref >= 0 && ScriptManager::getInstance() != 0){
		lua_State *L(ScriptManager::getInstance()->getState());
		lua_unref( L, ref);
		ref = -1;
	}
}

void TypeLua::get(lua_State* l) const{
	ASSERT( l != 0 );
	lua_getref(l, ref);
}

lua_State* TypeLua::getL() const{
	return ScriptManager::getInstance()->getState();
}