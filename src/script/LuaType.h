/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef _CHAOS_LUATYPE_H
#define _CHAOS_LUATYPE_H

#include "common.h"
//#include "traits.h"
#include "ScriptState.h"

struct lua_State;

_CHAOS_BEGIN

struct CHAOS_API TypeVoid{};
class ScriptThread;

/**
 * Object that is in lua is being exported out to CPP
 * The object is saved and referenced as an object (instance)
 */
class CHAOS_API LuaType{
public:
	LuaType(ScriptState const&, bool top = true);
	LuaType(LuaType const&);
	LuaType& operator=(LuaType const&);
	~LuaType();

	// test if ref is valid
	bool valid() const { return _ref != -1; };
	
	/**
	* push the reference onto the stack
	* @param
	*	l, where the value goes to
	*/
	void get(lua_State* l) const;

	int getRef() const { return _ref;};
	ScriptState const& getHost() const {return _host;};
	ScriptState& getHost() {return _host;};
    
private:
	int _ref;
    ScriptState _host;
};

//TYPE_NATIVE(TypeLua);
template<> void ScriptState::push_<LuaType>(lua_State *, const LuaType &val);
template<> LuaType ScriptState::get_<LuaType>(lua_State *, int idx);

// todo:
// LuaTable and LuaFunction helper classes or just integrate into TypeLua
_CHAOS_END

#endif
