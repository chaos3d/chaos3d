/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_UNWIND_H
#define	_CHAOS_UNWIND_H

#include "chaos_config.h"

_CHAOS_BEGIN


// Unwind qualifier const so we can only need to write a caster for regular type
// if you don't want to unwind const, specialize TypeUnwind for some const T
// for example
// template<>
// TypeUnwind<char const*>{
//	typedef char const*	type;
// };
// but this is generally not needed for any class.
template<class T> struct TypeUnwind						{ typedef	T	Type; };
template<class T> struct TypeUnwind<T const>			{ typedef	T	Type; };
template<class T> struct TypeUnwind<T const &>			{ typedef	T&	Type; };
template<class T> struct TypeUnwind<T const *>			{ typedef	T*	Type; };
template<class T> struct TypeUnwind<T const *&>			{ typedef	T*& Type; };
template<class T> struct TypeUnwind<T const * const>	{ typedef	T*	Type; };
template<class T> struct TypeUnwind<T const * const&>	{ typedef	T* const&	Type;	};
template<class T> struct TypeUnwind<T * const&>			{ typedef	T* const&	Type;	};
//	template<class T> struct TypeUnwind<T * const>{ typedef	T*	Type; };

// unwind const& of TypeLua directly into TypeLua to prevent copy constructor
class TypeLua;
template<> struct TypeUnwind<TypeLua const&> { typedef	TypeLua	Type; };

// don't unwind native string
template<> struct TypeUnwind<char const*>				{ typedef	char const*	Type;	};
template<> struct TypeUnwind<char const* const>			{ typedef	char const* const	Type;	};
template<> struct TypeUnwind<char const* const&>		{ typedef	char const* const&	Type;	};

_CHAOS_END

#endif

