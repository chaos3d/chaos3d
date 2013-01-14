/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_SCRIPTSTATE_H
#define	_CHAOS_SCRIPTSTATE_H

#include "chaos_config.h"
#include "LuaType.h"
#include "lua.h"
#include "unwind.h"
#include "core/core.h"
#include "core/decider.h"

#include <string>

_CHAOS_BEGIN

class ScriptManager;

/*
a running state of a specific script code
*/
class CHAOS_API ScriptState{
public:

	// Unwind the pointer and the reference
	// so we only need to implement the push pointer value for general class type
	// because c++ doesn't support partial specification of function template
	// we need to use class functor
	template<class T> struct GetUnwind;
	template<class T> struct PushUnwind;

protected:
	lua_State*		mL;

public:
	explicit ScriptState( lua_State* L )
		:mL(L)
	{};

	inline
	bool	operator ==(ScriptState const& rhs){
		return mL == rhs.mL;
	};

	inline
	bool	operator !=(ScriptState const& rhs){
		return mL != rhs.mL;
	};

	inline
	lua_State*	getState() const{
		return mL;
	}

	// to check if obj at index is the same as type1
	// or if it is derived from type
	template<class T>
	inline bool	checkType(int index, Type* type){
		return true;
	}

	/**
	* get generic object pointer
	*/
	void* getObject(int index) const;
		
	/**
	* push generic object pointer
	*/
	void pushObject(void* val, Type* type, bool gc) const;
		
	// 
	// return the strong type to be casted to any qualifier
	template<class T>
	inline T*	getValue(int index) const{
		return (T*)getObject(index);
	}

	template<class T>
	inline T	getNativeValue(int index) const;

	template<class T>
	inline T	get_( int index ) const;

	// specialize this to cast any native type to lua type
	template<class T>
	void	pushValue(T val, Type* type, bool gc) const;

	template<class T>
	inline void	pushNativeValue(T val) const;

	// TODO
	template<int type>
	inline void	wrapType(){
	}

	template<class T>
	inline void	push_(T val) const;

};

//template<> ReferencedCount* const& ScriptState::getValue<ReferencedCount>(int index);
template<> void ScriptState::pushValue<void*>(void* obj, Type* type, bool gc) const;
template<> void ScriptState::pushValue<ReferencedCount*>(ReferencedCount* obj, Type* type, bool gc) const;

#include "ScriptState.inl"

template<class T>
inline void	ScriptState::push_(T val) const{
	typedef typename TypeDecider<typename TypeUnwind<T>::Type>::TYPE RetType;
	typedef typename TypeTraits<typename TypeUnwind<T>::Type>::Type WrapType;
	PushUnwind<T>::push( *this, val, TYPE_FROM_NAME( WrapType ));
};

template<class T>
inline T	ScriptState::get_(int index) const{
	return GetUnwind<T>::get( *this, index );
};


_CHAOS_END

#endif
