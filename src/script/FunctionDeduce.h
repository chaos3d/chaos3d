/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef _CHAOS_FUNCTIONDEDUCE_H
#define _CHAOS_FUNCTIONDEDUCE_H

#include "LuaFunctor.h"

_CHAOS_BEGIN

// not a function type like NULL
template<class NotF>
CHAOS_API LuaFunctor* functionDeduce( NotF ){
	return 0;
}

template<typename T>
CHAOS_API LuaFunctor* accessorGDeduce( T* add ) {
	return new LuaGetter<T>((size_t)add);
}

template<typename T>
CHAOS_API LuaFunctor* accessorSDeduce( T* add ) {
	return new LuaSetter<T>((size_t)add);
}

template<typename T>
CHAOS_API LuaFunctor* arrayAccessorGDeduce(T* add, int r) {
	return new LuaArrayGetter<T>((size_t)add, r);
}

template<typename T>
CHAOS_API LuaFunctor* arrayAccessorSDeduce(T* add, int r) {
	return new LuaArraySetter<T>((size_t)add, r);
}

template<class R>
CHAOS_API LuaFunctor* functionDeduce( R(*pfn)() ){
	return new ApplierR<R>(pfn);
}

template<class R, class A1>
CHAOS_API LuaFunctor* functionDeduce( R(*pfn)(A1) ){
	return new ApplierRA1<R,A1>(pfn);
}

template<class R, class A1, class A2>
CHAOS_API LuaFunctor* functionDeduce( R(*pfn)(A1,A2) ){
	return new ApplierRA2<R,A1,A2>(pfn);
}

template<class R, class A1, class A2, class A3>
CHAOS_API LuaFunctor* functionDeduce( R(*pfn)(A1,A2,A3) ){
	return new ApplierRA3<R,A1,A2,A3>(pfn);
}

template<class R, class A1, class A2, class A3, class A4>
CHAOS_API LuaFunctor* functionDeduce( R(*pfn)(A1,A2,A3,A4) ){
	return new ApplierRA4<R,A1,A2,A3,A4>(pfn);
}

template<class R, class A1, class A2, class A3, class A4, class A5>
CHAOS_API LuaFunctor* functionDeduce( R(*pfn)(A1,A2,A3,A4,A5) ){
	return new ApplierRA5<R,A1,A2,A3,A4,A5>(pfn);
}

template<class R, class A1, class A2, class A3, class A4, class A5, class A6>
CHAOS_API LuaFunctor* functionDeduce( R(*pfn)(A1,A2,A3,A4,A5,A6) ){
	return new ApplierRA6<R,A1,A2,A3,A4,A5,A6>(pfn);
}

template<class R, class A1, class A2, class A3, class A4, class A5, class A6,class A7>
CHAOS_API LuaFunctor* functionDeduce( R(*pfn)(A1,A2,A3,A4,A5,A6,A7) ){
	return new ApplierRA7<R,A1,A2,A3,A4,A5,A6,A7>(pfn);
}

template<class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
CHAOS_API LuaFunctor* functionDeduce( R(*pfn)(A1,A2,A3,A4,A5,A6,A7,A8) ){
	return new ApplierRA8<R,A1,A2,A3,A4,A5,A6,A7,A8>(pfn);
}

template<class C, class R>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)() ){
	return new ApplierCR<C,R>(pfn);
}

template<class C, class R, class A1>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)(A1) ){
	return new ApplierCRA1<C,R,A1>(pfn);
}

template<class C, class R, class A1, class A2>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)(A1,A2) ){
	return new ApplierCRA2<C,R,A1,A2>(pfn);
}

template<class C, class R, class A1, class A2, class A3>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)(A1,A2,A3) ){
	return new ApplierCRA3<C,R,A1,A2,A3>(pfn);
}

template<class C, class R, class A1, class A2, class A3, class A4>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)(A1,A2,A3,A4) ){
	return new ApplierCRA4<C,R,A1,A2,A3,A4>(pfn);
}

template<class C, class R, class A1, class A2, class A3, class A4, class A5>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)(A1,A2,A3,A4,A5) ){
	return new ApplierCRA5<C,R,A1,A2,A3,A4,A5>(pfn);
}

template<class C, class R, class A1, class A2, class A3, class A4, class A5, class A6>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)(A1,A2,A3,A4,A5,A6) ){
	return new ApplierCRA6<C,R,A1,A2,A3,A4,A5,A6>(pfn);
}

template<class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)(A1,A2,A3,A4,A5,A6,A7) ){
	return new ApplierCRA7<C,R,A1,A2,A3,A4,A5,A6,A7>(pfn);
}

template<class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)(A1,A2,A3,A4,A5,A6,A7,A8) ){
	return new ApplierCRA8<C,R,A1,A2,A3,A4,A5,A6,A7,A8>(pfn);
}

// we use same functors for non-const and const functions
template<class C, class R>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)() const ){
	return new ApplierCR<C,R>(reinterpret_cast<R(C::*)()>(pfn));
}

template<class C, class R, class A1>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)(A1) const ){
	return new ApplierCRA1<C,R,A1>(reinterpret_cast<R(C::*)(A1)>(pfn));
}

template<class C, class R, class A1, class A2>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)(A1,A2) const ){
	return new ApplierCRA2<C,R,A1,A2>(reinterpret_cast<R(C::*)(A1,A2)>(pfn));
}

template<class C, class R, class A1, class A2, class A3>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)(A1,A2,A3) const ){
	return new ApplierCRA3<C,R,A1,A2,A3>(reinterpret_cast<R(C::*)(A1,A2,A3)>(pfn));
}

template<class C, class R, class A1, class A2, class A3, class A4>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)(A1,A2,A3,A4) const ){
	return new ApplierCRA4<C,R,A1,A2,A3,A4>(reinterpret_cast<R(C::*)(A1,A2,A3,A4)>(pfn));
}

template<class C, class R, class A1, class A2, class A3, class A4, class A5>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)(A1,A2,A3,A4,A5) const ){
	return new ApplierCRA5<C,R,A1,A2,A3,A4,A5>(reinterpret_cast<R(C::*)(A1,A2,A3,A4,A5)>(pfn));
}

template<class C, class R, class A1, class A2, class A3, class A4, class A5, class A6>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)(A1,A2,A3,A4,A5,A6) const ){
	return new ApplierCRA6<C,R,A1,A2,A3,A4,A5,A6>(reinterpret_cast<R(C::*)(A1,A2,A3,A4,A5,A6)>(pfn));
}

template<class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)(A1,A2,A3,A4,A5,A6,A7) const ){
	return new ApplierCRA7<C,R,A1,A2,A3,A4,A5,A6,A7>(reinterpret_cast<R(C::*)(A1,A2,A3,A4,A5,A6,A7)>(pfn));
}

template<class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
CHAOS_API LuaFunctor* functionDeduce( R(C::*pfn)(A1,A2,A3,A4,A5,A6,A7,A8) const ){
	return new ApplierCRA8<C,R,A1,A2,A3,A4,A5,A6,A7,A8>(reinterpret_cast<R(C::*)(A1,A2,A3,A4,A5,A6,A7,A8)>(pfn));
}

_CHAOS_END
#endif