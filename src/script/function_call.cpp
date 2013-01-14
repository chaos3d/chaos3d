/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "script/dispatcher.h"
#include "script/ScriptData.h"
#include "script/LuaFunctor.h"
#include "script/ScriptManager.h"
#include "script/ScriptThread.h"
#include "script/lua.h"
#include "script/bind.h"
#include "go/go.h"

_CHAOS_BEGIN

///------------------------------------------------------------------------
int	native_function_call( lua_State *L ){
#if 0
	ScriptData *data = (ScriptData*)lua_touserdata(L, lua_upvalueindex(2));
	ASSERT( data != 0 && data->dataType == ScriptData::Class );

	Type *type = data->class_;
	LuaFunctor *functor = 0;

	if( type->type == Type::FUNCTION && type->functor != 0 ){
		functor = static_cast<LuaFunctor*>(type->functor);
	}else if( type->type == Type::PROPERTY ){
		if( lua_gettop(L) == 0 )	// getter
			functor = static_cast<LuaFunctor*>(type->getter);
		else
			functor = static_cast<LuaFunctor*>(type->setter);
	}
	
	if( functor == 0 ){
		LOG("Not a function type, uable to call, return none.");
		return 0;
	}

	// if there is an obj in upvalue(1), insert into the bottom as a default obj
	// so you can just use the syntax
	// obj.foo
	// to call the method, instead
	// obj:foo
	// which is very error-prone

	// class call if value at upvalue index of 1 is function
	if( (lua_isfunction(L, lua_upvalueindex(1))) ){
		/**
		* @remark
		* class call can be static or super class call
		* it should depend on how it calls.
		* Class:method as a super class call
		* Class.method as a static call
		* so, it then depends on if the first parameter is a class def.
		*/
		// first parameter as the object

		ScriptData *classData = (ScriptData*)lua_touserdata(L, 1);
		// super class call, second argument as the object
		if( classData != 0 && classData->dataType == ScriptData::Class ){
			lua_pushvalue( L, 2 );
			lua_setupvalue( L, lua_upvalueindex(1), 1);
			lua_remove(L,1);
			lua_remove(L,1);
		}
	}

	data = (ScriptData*)lua_touserdata(L, lua_upvalueindex(1));
	if( data != 0 && data->obj == 0 ){	// only output a notice
		LOG("Call on a nil object. (%s)", ScriptManager::getInstance()->currentInfo().c_str());
	}
		
	functor->call(ScriptState(L));

#endif
	return ScriptManager::getInstance()->return_(1);
}

///------------------------------------------------------------------------
/*
int	native_property_call( lua_State *L ){
	ScriptData *data = (ScriptData*)lua_touserdata(L, lua_upvalueindex(1));
	ASSERT( data != 0 && data->dataType == ScriptData::Class );

	Type *type = data->class_;
	if(type->type != Type::PROPERTY ){
		LOG("Not a property type, uable to call, return none.");
		return 0;
	}

	bool setterNotGetter = lua_toboolean( L, lua_upvalueindex(3) ) == 1;

	LuaFunctor *functor = static_cast<LuaFunctor*>(setterNotGetter ? type->setter : type->getter);
	if( functor == 0 ){
		LOG("Unable to call the property (%s), return none", (setterNotGetter?"setter":"getter"));
		return 0;
	}

	if( !(lua_isnil(L, lua_upvalueindex(2))) ){
		lua_pushvalue( L, lua_upvalueindex(2) );
		lua_insert( L, 1 );
	}
		
	functor->call(&ScriptState(L));

	return ScriptManager::getInstance()->return_(1);
}*/

/*
static void lua_function_return(void* vSS, void* vCur){
	//ScriptThread* current((ScriptThread*)vCur);current->getState()
	ScriptThread* ss((ScriptThread*)vSS);
	lua_State* L((lua_State*)vCur);

	int ret = lua_gettop(ss->getState());
	lua_xmove(ss->getState(), L, ret);
	//current->release();
}
*/

/**
 * helper callback to save function results back to calling thread
 */
class LuaFuncRetTriggerable : public Triggerable{
	ScriptThread *mFrom, *mTo;

public:
	LuaFuncRetTriggerable( ScriptThread *from, ScriptThread *to )
		: mFrom(from), mTo(to)
	{};

	virtual void activated(){
		// always no errors? otherwise caught by lua and terminated?
		// if( lua_status(mFrom->getState()) == 0 )
		lua_xmove(mFrom->getState(), mTo->getState(), lua_gettop(mFrom->getState()));
	}
};

///------------------------------------------------------------------------
// todo: lua class call
// not supported yet
int lua_function_call( lua_State* L ){
#if 1
	int numarg = lua_gettop(L);
	if( lua_isfunction(L, lua_upvalueindex(1)) ){
		lua_pushvalue(L, 1);
		lua_setupvalue(L, 1, 1);
		numarg -= 1;
	}

	ScriptData* data = (ScriptData*)lua_touserdata(L, lua_upvalueindex(1));
	GOInstance* inst = (GOInstance*)data->obj;
	ASSERT(data->dataType == ScriptData::Instance);

	ScriptManager *sm = ScriptManager::getInstance();
	int ret(0);

	ScriptThread* ss(ScriptThread::thread( inst->getVar(lua_tostring( L, lua_upvalueindex(2))) ));
	if( ss == 0 ){
		LOG("Try to call a non-function on the object (%s [%8x]). return none. (%s)",
			lua_tostring(L,lua_upvalueindex(2)), inst->getClass(), sm->currentInfo().c_str());
		return 0;
	}
	
	lua_pushvalue( L, lua_upvalueindex(1));
	ss->setThis(TypeLua(L));

	lua_xmove( L, ss->getState(), numarg );
	if( !ss->run( true ) ){
		//current->retain();
		( (Triggerable*)(new LuaFuncRetTriggerable(ss, ScriptThread::current()))->autorelease())->wait4(ss);

		//ASSERT( sm->current() != 0 );
		ScriptThread::current()->wait4( ss);
	}else{
		ret = lua_gettop(ss->getState());
		lua_xmove(ss->getState(), L, ret);
	}
#else
	ScriptData* data = (ScriptData*)lua_touserdata(L, lua_upvalueindex(1));
	GOInstance* inst = (GOInstance*)data->obj;
	ASSERT(data->dataType == ScriptData::Instance);

	lua_pushvalue(L, lua_upvalueindex(2));
	TypeLua func(L);

	ScriptManager *sm = ScriptManager::getInstance();
	ScriptState* ss = sm->createState( func, false );
	if( ss == 0 ){
		LOG("Try to call a non-function on the object (%s [%8x]). return none. (%s)",
			lua_tostring(L,lua_upvalueindex(2)), inst->getClass(), sm->currentInfo().c_str());
		return 0;
	}
	
	lua_pushvalue( L, lua_upvalueindex(1));
	ss->setThis(TypeLua(L));

	int numarg = lua_gettop(L);
	lua_xmove( L, ss->getState(), numarg );
	int ret(0);
	if( !sm->run( ss ) ){
		sm->wait( ss, lua_function_return, L );
		sm->wait( ss );
	}else{
		ret = lua_gettop(ss->getState());
		lua_xmove(ss->getState(), L, ret);
	}
#endif
	return sm->return_(ret);
}
_CHAOS_END
