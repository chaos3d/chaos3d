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

#include "chaos_config.h"
#include "traits.h"
#include "core/ReferencedCount.h"

struct lua_State;

_CHAOS_BEGIN

struct CHAOS_API TypeVoid{};

/**
 * Object that is in lua is being exported out to CPP
 * The object is saved and referenced as an object (instance)
 */
class CHAOS_API TypeLua{
	int ref;

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

TYPE_NATIVE(TypeLua);

/**
 * A bridge class between objects in lua and objects in CPP
 * so that it's possible to export lua object out of CPP and
 * being referenced as a pointer
 *
 * this class is a helper class where objects when exporting
 * from lua implicitly by types (like function parameters) can
 * be seen identically to native classes. client should not
 * care about this
 */
class CHAOS_API LuaObject: public ReferencedCount{
	DECLARE_NOTCOPYABLE(LuaObject);
	DECLARE_NOTASSIGNABLE(LuaObject);
	
protected:
	TypeLua		mLua;
	
public:
	LuaObject(TypeLua const&);
};

// todo:
// LuaTable and LuaFunction helper classes or just integrate into TypeLua
_CHAOS_END

#endif
