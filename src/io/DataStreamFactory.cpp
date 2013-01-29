/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "io/DataStreamFactory.h"
#include "io/IOManager.h"
#include "io/DataStream.h"

//using namespace chaos;

void* DataStreamFactory::createLuaObject(lua_State* L){
	char const* url = luaL_checkstring( L, -1);

	DataStream* ds = IOManager::getInstance()->streamByURL( url );
	if( ds->valid() ){
		ds->retain();
		return ds;
	}else
		return 0;
}

void DataStreamFactory::destroyLuaObject(void* obj){
	((DataStream*)obj)->release();
}
