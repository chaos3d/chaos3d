/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "2d/PSOperator.h"
#include "script/bind.h"

using namespace ps;
//using namespace chaos;

IMPLEMENT_CLASS(ForceOperator, LIB_DOMAIN )
IMPLEMENT_FUNC(set, (void (ForceOperator::*)(float,float))&ForceOperator::set )
IMPLEMENT_END;

IMPLEMENT_CLASS(PSSpriteOperator, LIB_DOMAIN )
IMPLEMENT_END;

IMPLEMENT_CLASS(PSTransformOperator, LIB_DOMAIN )
IMPLEMENT_END;

IMPLEMENT_CLASS(VelocityOperator, LIB_DOMAIN )
IMPLEMENT_FUNC(angle, &VelocityOperator::setAngle )
IMPLEMENT_FUNC(velocity, &VelocityOperator::setVelocity )
IMPLEMENT_END;

IMPLEMENT_CLASS(AcceleratorOperator, LIB_DOMAIN )
IMPLEMENT_FUNC(radical, &AcceleratorOperator::setRadical )
IMPLEMENT_FUNC(tan, &AcceleratorOperator::setTan )
IMPLEMENT_END;


bool ForceOperator::parse(char const* key, lua_State* L){
	if( strcmp( key, "force" ) == 0 ){
		LUA_GET2(L);
		set( Vector2f((float)lua_tonumber( L, -2),(float)lua_tonumber( L, -1)) );
		return true;
	}else
		return false;
}

bool VelocityOperator::parse(char const* key, lua_State* L){
	if( strcmp( key, "velocity" ) == 0 ){
		LUA_GET4(L);
		setAngle((float)DEGREE_TO_RADIAN(lua_tonumber( L, -4)),
				 (float)DEGREE_TO_RADIAN(lua_tonumber( L, -3)));
		setVelocity((float)lua_tonumber( L, -2),(float)lua_tonumber( L, -1));
		return true;
	}else
		return false;
}

bool AcceleratorOperator::parse(char const* key, lua_State* L){
	if( strcmp( key, "accel" ) == 0 ){
		LUA_GET4(L);
		setRadical((float)lua_tonumber( L, -4), (float)lua_tonumber( L, -3));
		setTan((float)lua_tonumber( L, -2),(float)lua_tonumber( L, -1));
		return true;
	}else
		return false;
}

