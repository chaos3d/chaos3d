/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "ps/ParticleGenericOperator.h"
#include "script/bind.h"

//using namespace chaos::ps;
using namespace ps;

IMPLEMENT_CLASS(SizeOperator, LIB_DOMAIN )
IMPLEMENT_FUNC(set, &SizeOperator::set )
IMPLEMENT_END;

IMPLEMENT_CLASS(LifeOperator, LIB_DOMAIN )
IMPLEMENT_FUNC(set, &LifeOperator::set )
IMPLEMENT_END;

IMPLEMENT_CLASS(ColorOperator, LIB_DOMAIN )
IMPLEMENT_END;

bool SizeOperator::parse(char const* key, lua_State* L){
	if( strcmp( key, "size" ) == 0 ){
		lua_rawgeti( L, -1, 1 );lua_rawgeti( L, -2, 2 );
		lua_rawgeti( L, -3, 3 );lua_rawgeti( L, -4, 4 );
		set((float)lua_tonumber( L, -4),(float)lua_tonumber( L, -3),
			(float)lua_tonumber( L, -2),(float)lua_tonumber( L, -1));
		return true;
	}else
		return false;
}

void LifeOperator::set(float min_, float max_){
	mMinInv = 1.f/min_/30.f; // TODO Application::getInstance()->getDesiredFps();
	mMaxInv = 1.f/max_/30.f; // TODO Application::getInstance()->getDesiredFps();
}
bool LifeOperator::parse(char const* key, lua_State* L){
	if( strcmp( key, "life" ) == 0 ){
		lua_rawgeti( L, -1, 1 );lua_rawgeti( L, -2, 2 );
		set((float)lua_tonumber( L, -2), (float)lua_tonumber( L, -1));
		return true;
	}else
		return false;
}

bool ColorOperator::parse(char const* key, lua_State* L){
	if( strcmp( key, "color" ) == 0 ){
		ColorRGBA startLower(ColorRGBA::WHITE), startUpper(0.f,0.f,0.f,0.f),
			endLower(0.f,0.f,0.f,0.f), endUpper(0.f,0.f,0.f,0.f);
		ColorRGBA* values[4] = {&startLower, &startUpper, &endLower, &endUpper};
		for(int i=0;i<4;++i){
			lua_rawgeti( L, -1, i+1 );
			if( !lua_istable(L, -1) )
				break;
			lua_rawgeti( L, -1, 1 );lua_rawgeti( L, -2, 2 );
			lua_rawgeti( L, -3, 3 );lua_rawgeti( L, -4, 4 );
			*values[i] = ColorRGBA((float)lua_tonumber( L, -4), (float)lua_tonumber( L, -3),
				(float)lua_tonumber( L, -2),(float)lua_tonumber( L, -1));
			lua_pop(L,5);
		}
		set( startLower, startUpper, endLower, endUpper );
		return true;
	}else
		return false;
}
