/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_LUAFUNCTOR_H
#define	_CHAOS_LUAFUNCTOR_H

#include "chaos_config.h"

#if defined(__OBJC__)
#include <objc/message.h>
#include <objc/runtime.h>
#endif

#include "ScriptManager.h"
#include "ScriptState.h"
#include "ScriptData.h"
#include "dispatcher.h"
#include "lua.h"
#include "traits.h"
#include "unwind.h"

#include <string>

struct lua_State;

#define __LUA_CALL
_CHAOS_BEGIN

/*
A script functor, binding to native ones with lua
*/
class CHAOS_API LuaFunctor{

	DECLARE_NOTCOPYABLE(LuaFunctor);
	DECLARE_NOTASSIGNABLE(LuaFunctor);
public:
	typedef int (*LuaFunc)(lua_State*);
	
protected:
/*	class ScriptStateWrapper : public ScriptState{
	public:
		ScriptStateWrapper(lua_State *L) : ScriptState(L,-1){};
	};
*/
	template<typename T>
	struct TypeName;

	template<typename T>
	struct QualifierName;

	template<typename T>
	struct TypeName{
		inline static char const* name(){
			typedef typename TypeTraits<typename TypeUnwind<T>::Type>::Type WrapType;
			Type* retType = TYPE_FROM_NAME( WrapType );
			if( retType == 0)
				return "Unknown type";
			else
				return retType->typeName();
		}

	};

	template<typename T>
	struct QualifierName{
		inline static char const* name(){
			return "";
		};
	};

protected:
	template<typename T>
	inline static std::string typeName(){
		return std::string(TypeName<T>::name()) + QualifierName<T>::name();
	}

	LuaFunc	luaCall;
	
public:
	LuaFunctor(LuaFunc _call) : luaCall(_call){};
	~LuaFunctor(){};
	
	inline LuaFunc getFunc() const { return luaCall; };
	inline void* getNativeFunc() const { return 0;};//funcPtr; };
	//virtual void call(ScriptState const& L ) = 0;
	virtual std::string	description(){
		return std::string("(no description)");
	};
	
	//template<typename F>
	static int lua_call(lua_State* L){
		//ScriptData *data = (ScriptData*)lua_touserdata(L, lua_upvalueindex(2));
		//((F*)data->type->functor)->F::call(ScriptState(L));		
		//return ScriptManager::getInstance()->return_(1);
		return 0;
	}
};

#define TYPENAME(type)	\
		template<> struct LuaFunctor::TypeName<type>{	\
			inline static char const* name(){ return #type; }	\
		};

TYPENAME(bool);
TYPENAME(char);
TYPENAME(int);
TYPENAME(short);
TYPENAME(long);
TYPENAME(float);
TYPENAME(void);
TYPENAME(double);

template<> struct LuaFunctor::TypeName<std::string>{	\
	inline static char const* name(){ return "string"; }	\
};

template<> struct LuaFunctor::TypeName<void*>{	\
	inline static char const* name(){ return "[any(void)]"; }	\
};

// leave the qualifiers
template<> struct LuaFunctor::TypeName<char const*>{	\
	inline static char const* name(){ return "char"; }	\
};

template<> struct LuaFunctor::TypeName<TypeLua>{	\
	inline static char const* name(){ return "lua ref"; }	\
};

template<> struct LuaFunctor::TypeName<TypeLua const>{	\
	inline static char const* name(){ return "lua ref"; }	\
};

template<> struct LuaFunctor::TypeName<TypeLua const&>{	\
	inline static char const* name(){ return "lua ref"; }	\
};

#define QUALIFIERNAME(q)	\
		template<typename T> struct LuaFunctor::QualifierName<T q>{	\
			inline static char const* name(){ return " " #q; }	\
		};

QUALIFIERNAME(&);
QUALIFIERNAME(const);
QUALIFIERNAME(const&);
QUALIFIERNAME(*);
QUALIFIERNAME(const*);
QUALIFIERNAME(*const);
QUALIFIERNAME(const*const);
QUALIFIERNAME(*&);
QUALIFIERNAME(const*&);
QUALIFIERNAME(*const&);
QUALIFIERNAME(const*const&);

#undef TYPENAME
#undef QUALIFIERNAME


///----------------------------------------------------------------
///----------------------------------------------------------------
///----------------------------------------------------------------
template<class T>
class CHAOS_API ApplierCopy : public LuaFunctor{
public:
	virtual	void	call(ScriptState const& sL ){
		lua_State* L = sL.getState();
		ScriptData *data = (ScriptData*)lua_touserdata(L, 1);

		ASSERT( data->dataType == ScriptData::Instance && data->obj != 0);

		ScriptData *objData = (ScriptData*)lua_newuserdata( L, sizeof(ScriptData));
		objData->dataType = ScriptData::Instance;
		objData->type = data->type;
		objData->obj = new T(*(T*)data->obj);

		native_instgc_meta( L );
		lua_setmetatable( L, -2 );
	}

};


////////////////
// no arguments
template<class R>
class CHAOS_API ApplierR : public LuaFunctor{
protected:
	typedef		ApplierR<R>	SELF;
	typedef		R (*TFnPtr)();
	TFnPtr		mPtr;

public:
	ApplierR( TFnPtr ptr ): LuaFunctor(SELF::lua_call), mPtr(ptr){
	};

#if defined(__LUA_CALL)
	static int lua_call(lua_State* _L);
#else
	virtual	void call(ScriptState const& L);
#endif
	virtual std::string	description(){
		return typeName<R>()+"(*)()";
	};

};

#if defined(__LUA_CALL)
template<>
int ApplierR<void>::lua_call(lua_State* _L);
#else
template<>
void ApplierR<void>::call(ScriptState const& L);
#endif

#if defined(__LUA_CALL)
template<class R>
int ApplierR<R>::lua_call(lua_State* _L){
	ScriptState L(_L);
	SELF*		thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));
#else
template<class R>
void ApplierR<R>::call(ScriptState const& L){
#endif
	typedef typename TypeTraits<typename TypeUnwind<R>::Type>::Type WrapType;
	typedef typename TypeDecider<typename TypeUnwind<R>::Type>::TYPE	RetType;

	ScriptState::PushUnwind<R>::push(
		L,
		(thiz->mPtr)(),
		TYPE_FROM_NAME( WrapType )
		);
#if defined(__LUA_CALL)
	return ScriptManager::getInstance()->return_(1);
#endif
};

////////////////
// class, no arguments
template<class C,class R>
class ApplierCR : public LuaFunctor{
protected:
	typedef		ApplierCR<C,R>	SELF;
	typedef		R (C::*TFnPtr)();
	TFnPtr		mPtr;

public:
	ApplierCR(TFnPtr ptr): LuaFunctor(SELF::lua_call), mPtr(ptr){
	};

#if defined(__LUA_CALL)
	static int lua_call(lua_State* _L){
		ScriptState L(_L);
		SELF*		thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));
#else
	virtual	void call(ScriptState const& L ){
#endif
		typedef typename TypeTraits<typename TypeUnwind<C>::Type>::Type CheckType;
		typedef typename TypeTraits<typename TypeUnwind<R>::Type>::Type WrapType;
		typedef typename TypeDecider<typename TypeUnwind<R>::Type>::TYPE RetType;

		if( CHECK_TYPE( 
			L->checkType( 1, TYPE_FROM_NAME(CheckType)) 
			) )
		{
			 // only use void* to reuse template class
			C* obj = (C*)ScriptState::GetUnwind<void*>::get( L, lua_upvalueindex(1));

			// it will use void* for any pointer type
			// and then wrap a class type as the class name
			ScriptState::PushUnwind<R>::push(
				L,
				(obj->*(thiz->mPtr))(),
				TYPE_FROM_NAME( WrapType )
				);
		}else{
			LOG("Object type doesn't match, uable to call methods. return nil.");
			lua_pushnil(L.getState());
		}
#if defined(__LUA_CALL)
		return ScriptManager::getInstance()->return_(1);
#endif
	}

	virtual std::string	description(){
		return typeName<R>() + " ("
			+ typeName<C>() + "::*)()";
	};

};

// class, return void
template<class C>
class ApplierCR<C, void> : public LuaFunctor{
protected:
	typedef		ApplierCR<C,void>	SELF;
	typedef		void (C::*TFnPtr)();
	TFnPtr		mPtr;

public:
	ApplierCR(TFnPtr ptr): LuaFunctor(SELF::lua_call), mPtr(ptr){
	};

#if defined(__LUA_CALL)
	static int lua_call(lua_State* _L){
		ScriptState L(_L);
		SELF*		thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));
#else
	virtual	void call(ScriptState const& L ){
#endif
		typedef typename TypeTraits<typename TypeUnwind<C>::Type>::Type CheckType;

		if( CHECK_TYPE( 
			L->checkType( 1, TYPE_FROM_NAME(CheckType)) 
			) )
		{
			C* obj = (C*)ScriptState::GetUnwind<void*>::get( L, lua_upvalueindex(1));
			(obj->*(thiz->mPtr))();
		}else{
			LOG("Object type doesn't match, uable to call methods. return nil.");
		}
#if defined(__LUA_CALL)
		return ScriptManager::getInstance()->return_(0);
#endif
	}

	virtual std::string	description(){
		return typeName<void>() + " ("
			+ typeName<C>() + "::*)()";
	};

};

#include "LuaAccessor.inl"
#include "LuaFunctor1.inl"
#include "LuaFunctor2.inl"
#include "LuaFunctor3.inl"
#include "LuaFunctor4.inl"
#include "LuaFunctor5.inl"
#include "LuaFunctor6.inl"
#include "LuaFunctor7.inl"
#include "LuaFunctor8.inl"

// Wrapper for OBJ-C selectors
#if defined(__OBJC__)
#include "LuaFunctor1-oc.inl"
#include "LuaFunctor2-oc.inl"
#endif
_CHAOS_END

#endif

