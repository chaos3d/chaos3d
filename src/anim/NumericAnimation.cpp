/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "anim/NumericAnimation.h"
#include "anim/TimingEquation.h"

#include "core/core.h"
#include "script/bind.h"
#include "script/lua.h"

#include <algorithm>

_CHAOS_BEGIN

template<class KeyFrame>
struct SortFunctor{
	inline bool operator()(KeyFrame const& lhs, KeyFrame const& rhs){
		return lhs.stamp < rhs.stamp;
	}
};

template<>
NumericAnimation<float>::NumericAnimation(TypeLua const& lua) : mLoop(false){
	lua_State* L = lua.getL();

	int n = lua_gettop( L );
	do{
		lua.get(L);
		if( !lua_istable( L, -1 ) ){
			LOG("Expected a table. (ValueAnimation)");
			break;
		}

		lua_pushnil( L );
		TKFList keyframes;
		keyframes.reserve(16);
		while( lua_next( L, -2 ) ){
			if( lua_type(L,-2) == LUA_TNUMBER ){
				keyframes.push_back( KeyFrame((float) lua_tonumber(L,-1), (float) lua_tonumber(L,-1)));
			}else if( lua_type(L,-1) == LUA_TBOOLEAN ){
				if( lua_type(L,-2) == LUA_TSTRING &&
					strcmp(lua_tostring(L,-2), "loop") == 0 )
					mLoop = lua_toboolean(L, -1) == 1;
				lua_pop(L,1);
			}

		}

		std::sort( keyframes.begin(), keyframes.end(), SortFunctor<KeyFrame>() );
		mKeyFrames = keyframes;

	}while(0);

	lua_settop(L, n );
}

template<>
NumericAnimation<Vector2f>::NumericAnimation(TypeLua const& lua) : mLoop(false){
	lua_State* L = lua.getL();

	int n = lua_gettop( L );
	do{
		lua.get(L);
		if( !lua_istable( L, -1 ) ){
			LOG("Expected a table. (Vector2fAnimation)");
			break;
		}

		lua_pushnil( L );
		TKFList keyframes;
		keyframes.reserve(16);
		while( lua_next( L, -2 ) ){
			if( lua_type(L,-2) == LUA_TNUMBER ){
				if( lua_type(L,-1) == LUA_TTABLE ){
					LUA_GET2(L);
					keyframes.push_back( 
						KeyFrame( 
						Vector2f( (float)lua_tonumber(L, -2),( float)lua_tonumber(L, -1)),
						(float) lua_tonumber(L,-4)
						)
					);
					lua_pop(L,4);
				}else if( lua_type(L,-1) == LUA_TUSERDATA ){
					ScriptData* data = (ScriptData*)lua_touserdata(L, -1);
					if( data->obj == 0 || data->dataType != ScriptData::Instance ){ //todo type checking
						LOG("Vector2f not applicable. (Vector2fAnimation)");
					}else{
						keyframes.push_back( 
							KeyFrame( 
							*(Vector2f*)data->obj,
							(float) lua_tonumber(L,-2)
							)
						);
					}
					lua_pop(L,1);
				}else
					lua_pop(L,1);

			}else if( lua_type(L,-1) == LUA_TBOOLEAN ){
				if( lua_type(L,-2) == LUA_TSTRING &&
					strcmp(lua_tostring(L,-2), "loop") == 0 )
					mLoop = lua_toboolean(L, -1) == 1;
				lua_pop(L,1);
			}

		}

		std::sort( keyframes.begin(), keyframes.end(), SortFunctor<KeyFrame>() );
		mKeyFrames = keyframes;

	}while(0);

	lua_settop(L, n );
}

template<>
NumericAnimation<Vector3f>::NumericAnimation(TypeLua const& lua) : mLoop(false){
	lua_State* L = lua.getL();

	int n = lua_gettop( L );
	do{
		lua.get(L);
		if( !lua_istable( L, -1 ) ){
			LOG("Expected a table. (Vector3fAnimation)");
			break;
		}

		lua_pushnil( L );
		TKFList keyframes;
		keyframes.reserve(16);
		while( lua_next( L, -2 ) ){
			if( lua_type(L,-2) == LUA_TNUMBER ){
				if( lua_type(L,-1) == LUA_TTABLE ){
					lua_rawgeti( L, -1, 1);	lua_rawgeti( L, -2, 2);	lua_rawgeti( L, -3, 3);
					keyframes.push_back( 
						KeyFrame( 
						Vector3f( (float)lua_tonumber(L, -3),(float)lua_tonumber(L, -2),( float)lua_tonumber(L, -1)),
						(float) lua_tonumber(L,-5)
						)
					);
					lua_pop(L,4);
				}else if( lua_type(L,-1) == LUA_TUSERDATA ){
					ScriptData* data = (ScriptData*)lua_touserdata(L, -1);
					if( data->obj == 0 || data->dataType != ScriptData::Instance ){ //todo type checking
						LOG("Vector3f not applicable. (Vector3fAnimation)");
					}else{
						keyframes.push_back( 
							KeyFrame( 
							*(Vector3f*)data->obj,
							(float) lua_tonumber(L,-2)
							)
						);
					}
					lua_pop(L,1);
				}else
					lua_pop(L,1);

			}else if( lua_type(L,-1) == LUA_TBOOLEAN ){
				if( lua_type(L,-2) == LUA_TSTRING &&
					strcmp(lua_tostring(L,-2), "loop") == 0 )
					mLoop = lua_toboolean(L, -1) == 1;
				lua_pop(L,1);
			}

		}

		std::sort( keyframes.begin(), keyframes.end(), SortFunctor<KeyFrame>() );
		mKeyFrames = keyframes;

	}while(0);

	lua_settop(L, n );
}

template<>
NumericAnimation<Quaternionf>::NumericAnimation(TypeLua const& lua) : mLoop(false){
	lua_State* L = lua.getL();

	int n = lua_gettop( L );
	do{
		lua.get(L);
		if( !lua_istable( L, -1 ) ){
			LOG("Expected a table. (QuaternionAnimation)");
			break;
		}

		lua_pushnil( L );
		TKFList keyframes;
		keyframes.reserve(16);
		while( lua_next( L, -2 ) ){
			if( lua_type(L,-2) == LUA_TNUMBER ){
				if( lua_type(L,-1) == LUA_TTABLE ){
					keyframes.push_back( KeyFrame( (float) lua_tonumber(L,-2) )	);

					LUA_GET4(L);
					if(lua_isnoneornil(L,-1)){
						keyframes.back().value.FromEulerAngle(
							(float)DEGREE_TO_RADIAN(lua_tonumber(L, -4)),
							(float)DEGREE_TO_RADIAN(lua_tonumber(L, -3)),
							(float)DEGREE_TO_RADIAN(lua_tonumber(L, -2))
						);
					}else{
						keyframes.back().value.FromAxisAngle(
							Vector3f(
							(float)lua_tonumber(L, -4),
							(float)lua_tonumber(L, -3),
							(float)lua_tonumber(L, -2)),
							(float)DEGREE_TO_RADIAN(lua_tonumber(L, -1))
						);
					}
					lua_pop(L,5);
				}else if( lua_type(L,-1) == LUA_TUSERDATA ){
					ScriptData* data = (ScriptData*)lua_touserdata(L, -1);
					if( data->obj == 0 || data->dataType != ScriptData::Instance ){ //todo type checking
						LOG("Quaternionf not applicable. (QuaternionAnimation)");
					}else{
						keyframes.push_back( 
							KeyFrame( 
							*(Quaternionf*)data->obj,
							(float) lua_tonumber(L,-2)
							)
						);
					}
					lua_pop(L,1);
				}else
					lua_pop(L,1);

			}else if( lua_type(L,-1) == LUA_TBOOLEAN ){
				if( lua_type(L,-2) == LUA_TSTRING &&
					strcmp(lua_tostring(L,-2), "loop") == 0 )
					mLoop = lua_toboolean(L, -1) == 1;
				lua_pop(L,1);
			}

		}

		std::sort( keyframes.begin(), keyframes.end(), SortFunctor<KeyFrame>() );
		mKeyFrames = keyframes;

	}while(0);

	lua_settop(L, n );
}

template<>
NumericAnimation<ColorRGBA>::NumericAnimation(TypeLua const& lua) : mLoop(false){
	lua_State* L (lua.getL());

	int n = lua_gettop( L );
	do{
		lua.get(L);
		if( !lua_istable( L, -1 ) ){
			LOG("Expected a table. (ColorAnimation)");
			break;
		}

		lua_pushnil( L );
		TKFList keyframes;
		keyframes.reserve(16);
		while( lua_next( L, -2 ) ){
			if( lua_type(L,-2) == LUA_TNUMBER ){
				if( lua_type(L,-1) == LUA_TTABLE ){
					lua_rawgeti( L, -1, 1);	lua_rawgeti( L, -2, 2);
					lua_rawgeti( L, -3, 3);	lua_rawgeti( L, -4, 4);
					keyframes.push_back( 
						KeyFrame( 
							ColorRGBA( (float)lua_tonumber(L, -4),(float)lua_tonumber(L, -3),
							(float)lua_tonumber(L, -2),( float)lua_tonumber(L, -1)),
						(float) lua_tonumber(L,-6)
						)
					);
					lua_pop(L,5);
				}else if( lua_type(L,-1) == LUA_TUSERDATA ){
					ScriptData* data = (ScriptData*)lua_touserdata(L, -1);
					if( data->obj == 0 || data->dataType != ScriptData::Instance ){ //todo type checking
						LOG("ColorRGBA not applicable. (ColorAnimation)");
					}else{
						keyframes.push_back( 
							KeyFrame( 
							*(ColorRGBA*)data->obj,
							(float) lua_tonumber(L,-2)
							)
						);
					}
					lua_pop(L,1);
				}else
					lua_pop(L,1);

			}else if( lua_type(L,-1) == LUA_TBOOLEAN ){
				if( lua_type(L,-2) == LUA_TSTRING &&
					strcmp(lua_tostring(L,-2), "loop") == 0 )
					mLoop = lua_toboolean(L, -1) == 1;
				lua_pop(L,1);
			}

		}

		std::sort( keyframes.begin(), keyframes.end(), SortFunctor<KeyFrame>() );
		mKeyFrames = keyframes;

	}while(0);

	lua_settop(L, n );
}

template class NumericAnimation<Vector3f>;
template class NumericAnimation<Vector2f>;
template class NumericAnimation<Quaternionf>;
template class NumericAnimation<ColorRGBA>;
template class NumericAnimation<float>;

/*
struct TranslateApplierWrapper{
	typedef SceneNode::TransformApplier		Applier;
	typedef SceneNode	Appliee;
};

struct RotationApplierWrapper{
	typedef SceneNode::RotationApplier		Applier;
	typedef SceneNode	Appliee;
};

struct ScaleApplierWrapper{
	typedef SceneNode::ScaleApplier	Applier;
	typedef SceneNode	Appliee;
};


struct ColorApplierWrapper{
	typedef UINode::ColorApplier	Applier;
	typedef UINode	Appliee;
};
	*/
/*
IMPLEMENT_CLASS(ValueAnimationState, LIB_DOMAIN )
IMPLEMENT_END;
IMPLEMENT_CLASS(QuaternionAnimationState, LIB_DOMAIN )
IMPLEMENT_END;
IMPLEMENT_CLASS(ColorAnimationState, LIB_DOMAIN )
IMPLEMENT_END;
IMPLEMENT_CLASS(Vector3fAnimationState, LIB_DOMAIN )
IMPLEMENT_END;
IMPLEMENT_CLASS(Vector2fAnimationState, LIB_DOMAIN )
IMPLEMENT_END;
*/
IMPLEMENT_CLASS(ValueAnimation, LIB_DOMAIN )
IMPLEMENT_FACTORY( (LuaRefFactory<ValueAnimation>) )
IMPLEMENT_END;

IMPLEMENT_CLASS(Vector2fAnimation, LIB_DOMAIN )
IMPLEMENT_FACTORY( (LuaRefFactory<Vector2fAnimation>) )
IMPLEMENT_END;

IMPLEMENT_CLASS(Vector3fAnimation, LIB_DOMAIN )
IMPLEMENT_FACTORY( (LuaRefFactory<Vector3fAnimation>) )
/*
IMPLEMENT_FUNC( nodeTranslate, (&Vector3fAnimation::animation<TranslateApplierWrapper, LinearTiming>) )
IMPLEMENT_FUNC( nodeTranslate3, (&Vector3fAnimation::animation<TranslateApplierWrapper, CubicTiming>) )
IMPLEMENT_FUNC( nodeScale, (&Vector3fAnimation::animation<ScaleApplierWrapper, LinearTiming>) )
IMPLEMENT_FUNC( nodeScale3, (&Vector3fAnimation::animation<ScaleApplierWrapper, CubicTiming>) )
*/
IMPLEMENT_END;

IMPLEMENT_CLASS(QuaternionAnimation, LIB_DOMAIN  )
IMPLEMENT_FACTORY( (LuaRefFactory<QuaternionAnimation>) )
/*IMPLEMENT_FUNC( nodeRotate, (&QuaternionAnimation::animation<RotationApplierWrapper, LinearTiming>) )
IMPLEMENT_FUNC( nodeRotate3, (&QuaternionAnimation::animation<RotationApplierWrapper, CubicTiming>) )
*/
IMPLEMENT_END;

IMPLEMENT_CLASS(ColorAnimation, LIB_DOMAIN  )
IMPLEMENT_FACTORY( (LuaRefFactory<ColorAnimation>) )
/*IMPLEMENT_FUNC( uiColor, (&ColorAnimation::animation<ColorApplierWrapper, LinearTiming>) )
IMPLEMENT_FUNC( uiColor3, (&ColorAnimation::animation<ColorApplierWrapper, CubicTiming>) )
*/
IMPLEMENT_END;
_CHAOS_END