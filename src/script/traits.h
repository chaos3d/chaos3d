/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_TRAITS_H
#define	_CHAOS_TRAITS_H

#include "chaos_config.h"

_CHAOS_BEGIN

class Type;

class TypeNative{
public:
	static Type* __getType(){
		return 0;
	};
};

template<typename T>
class TypeApplier{
public:
	inline
	static Type* getType(){
		return T::__getType();
	}
};
/*
this is to transform native type, like int/char to class type
*/
template<class T> struct TypeTraits{	typedef	T	Type; };
template<class T> struct TypeTraits<T*>{	typedef	T	Type; };
template<class T> struct TypeTraits<T&>{	typedef	T	Type; };

#define TYPE_NATIVE( type )			\
	template<> struct TypeTraits<type>{	typedef	TypeNative	Type; };		\
	template<> struct TypeTraits<type*>{	typedef	TypeNative	Type; };	\
	template<> struct TypeTraits<type&>{	typedef	TypeNative	Type; };	

TYPE_NATIVE(const char);
TYPE_NATIVE(char);
TYPE_NATIVE(bool);
TYPE_NATIVE(short);
TYPE_NATIVE(int);
TYPE_NATIVE(long);
TYPE_NATIVE(float);
TYPE_NATIVE(double);
TYPE_NATIVE(std::string);
template<> struct TypeTraits<void>{	typedef	TypeNative	Type; };		\
template<> struct TypeTraits<void*>{	typedef	TypeNative	Type; };	\

_CHAOS_END

#endif
