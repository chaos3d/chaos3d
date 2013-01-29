/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_DATASTREAMFACTORY_H
#define	_CHAOS_DATASTREAMFACTORY_H

#include "chaos_config.h"
#include "script/LuaFactory.h"

_CHAOS_BEGIN

class DataStreamFactory : public LuaObjectFactory{
public:
	virtual	void*		createLuaObject( lua_State* L);
	virtual void		destroyLuaObject(void* obj);
};

_CHAOS_END

#endif
