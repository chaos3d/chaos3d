/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_LUAFACTORY_H
#define	_CHAOS_LUAFACTORY_H

#include "chaos_config.h"
#include "lua.h"
#include "LuaType.h"

_CHAOS_BEGIN

/*
A factory class to construct and destruct objects in the script
*/
class CHAOS_API LuaObjectFactory{
	DECLARE_NOTCOPYABLE(LuaObjectFactory);
	DECLARE_NOTASSIGNABLE(LuaObjectFactory);

public:
	LuaObjectFactory(){};
	virtual ~LuaObjectFactory(){};

	//destroy the object
	virtual void	destroyLuaObject( void* obj ) = 0;

	// create an object
	virtual	void*	createLuaObject( lua_State* L) = 0;

};

/*
default factory using new/delete operator
*/
template<class T>
class CHAOS_API LuaCreator : public LuaObjectFactory{
public:
	virtual	void*	createLuaObject( lua_State* L){
		return new T();
	}

	virtual	void	destroyLuaObject( void* obj ){
		delete (T*)obj;
	}
};

/*
the factory using lua table for referenced count objects
*/
template<class Ref>
class CHAOS_API LuaRefFactory : public LuaObjectFactory{
public:
	virtual	void*	createLuaObject( lua_State* L){
		lua_pushvalue(L, 2);
		Ref *node = new Ref( TypeLua(lua_ref(L,1)));
		return node;
	}

	virtual	void	destroyLuaObject( void* obj ){
		((Ref*) obj)->release();
	}
};

/*
the factory using lua table for general objects
*/
template<class T>
class CHAOS_API LuaObjFactory : public LuaObjectFactory{
public:
	virtual	void*	createLuaObject( lua_State* L){
		lua_pushvalue(L, 2);
		TypeLua lua(lua_ref(L,1));
		T *node = new T( lua );
		lua_unref( L, lua.ref );
		return node;
	}

	virtual	void	destroyLuaObject( void* obj ){
		delete (T*) obj;
	}
};

_CHAOS_END

#endif


