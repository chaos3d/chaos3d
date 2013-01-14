/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "script/LuaFunctor.h"
#include "script/FunctionDeduce.h"

//using namespace chaos;

#if defined(__LUA_CALL)
template<>
int ApplierR<void>::lua_call(lua_State *_L){
	ScriptState L(_L);
	SELF*		thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));
#else
template<>
void ApplierR<void>::call(ScriptState const& L ){
#endif
	(thiz->mPtr)();
#if defined(__LUA_CALL)
	return ScriptManager::getInstance()->return_(0);
#endif
};

#if defined(TEST_LUA_BIND)

class Test{
public:
	int get(void) const{ return 0;};
};
TYPE_NATIVE(Test);

template ApplierCR<Test, Test &>;

template ApplierR<int>;
template ApplierCRA2<Test,Test*,int,const char*>;
template ApplierR<Test>;
template ApplierR<Test const>;
//template ApplierR<Test *const&>;
template ApplierR<Test&>;
template ApplierR<Test*>;
template ApplierR<Test*&>;
template ApplierR<Test* const&>;
template ApplierR<Test const * const>;
template ApplierR<Test const * const&>;
template ApplierR<Test* const>;

template ApplierCR<Test, int*>;
template ApplierCR<Test, Test>;
template ApplierCRA1<Test, void, char const*>;
template ApplierCRA1<Test, Test *, int>;
template ApplierCRA1<Test, int, Test &>;
template ApplierCRA1<Test, int, Test const >;
template ApplierCRA1<Test, int, Test const &>;
template ApplierCRA1<Test, int, Test const * >;
template ApplierCRA1<Test, int, Test const * const>;
template ApplierCRA1<Test, int, Test const * const &>;
template ApplierCRA1<Test, int, Test * const>;
template ApplierCRA1<Test, int, Test * const &>;

template ApplierRA1<void, int>;
template ApplierRA1<int, Test const&>;
//template ApplierRA1<Test const>;
//template ApplierRA1<Test *const&>;

void test(){
	int const i = 4;
	//int 
	functionDeduce(&Test::get);
	Test *t;
	ScriptState::NilState.pushValue<void>(t, 0, true);
}

#endif