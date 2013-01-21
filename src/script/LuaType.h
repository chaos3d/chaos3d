/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef _CHAOS_LUAOBJECT_H
#define _CHAOS_LUAOBJECT_H

#include "common.h"
//#include "traits.h"

struct lua_State;

_CHAOS_BEGIN

struct CHAOS_API TypeVoid{};
class ScriptThread;

/**
 * Object that is in lua is being exported out to CPP
 * The object is saved and referenced as an object (instance)
 */
class CHAOS_API TypeLua{
	int ref;
    ScriptThread* mHost;
    
public:
	explicit TypeLua(int r = -1):ref(r){};
	explicit TypeLua(lua_State*);
	TypeLua(TypeLua const&);
	TypeLua& operator=(TypeLua const&);
	~TypeLua();

	// test if ref is valid
	bool valid() const { return ref != -1; };
	
	/**
	* push the reference onto the stack
	* @param
	*	l, where the value goes to
	*/
	void get(lua_State* l) const;

	int getRef() const { return ref;};
	lua_State* getL() const;
};

//TYPE_NATIVE(TypeLua);

// todo:
// LuaTable and LuaFunction helper classes or just integrate into TypeLua
_CHAOS_END

#endif
