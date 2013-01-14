/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "script/dispatcher.h"
#include "script/lua.h"

#include "core/core.h"

_CHAOS_BEGIN

void native_class_meta( lua_State *L ){
	luaL_getmetatable( L, "__meta_class" );
	if( lua_isnil( L, -1 ) ){
		lua_pop( L, 1); //pop nil

		int ret = luaL_newmetatable( L, "__meta_class" );
		ASSERT( ret == 1 );
	
		lua_pushstring( L, "__index" );				// return a function closure with upvalue(2)=nil
		lua_pushcclosure( L, native_class_index, 0 );
		lua_rawset( L, -3 );

		lua_pushstring( L, "__call" );				// call the factory method
		lua_pushcclosure( L, native_class_call, 0 ); 
		lua_rawset( L, -3 );

		lua_pushstring( L, "__tostring" );			// debug output
		lua_pushcclosure( L, lua_meta_tostring, 0 ); 
		lua_rawset( L, -3 );
	}
}

void native_instgc_meta( lua_State *L ){
	luaL_getmetatable( L, "__meta_managed_instance" );
	if( lua_isnil( L, -1 ) ){
		lua_pop( L, 1);

		int ret = luaL_newmetatable( L, "__meta_managed_instance" );
		ASSERT( ret == 1);
	
		lua_pushstring( L, "__index" );				// return a function closure with upvalue(2)=obj
		lua_pushcclosure( L, native_inst_index, 0 ); 
		lua_rawset( L, -3 );

		lua_pushstring( L, "__newindex" );
		lua_pushcclosure( L, native_inst_newindex, 0 ); 
		lua_rawset( L, -3 );

		lua_pushstring( L, "__gc" );				// call the destroy method
		lua_pushcclosure( L, native_inst_gc, 0 );
		lua_rawset( L, -3 );

		lua_pushstring( L, "__tostring" );			// debug output
		lua_pushcclosure( L, lua_meta_tostring, 0 ); 
		lua_rawset( L, -3 );
	}
}

void native_instref_meta( lua_State *L ){
	luaL_getmetatable( L, "__meta_managed_ref_instance" );
	if( lua_isnil( L, -1 ) ){
		lua_pop( L, 1);

		int ret = luaL_newmetatable( L, "__meta_managed_ref_instance" );
		ASSERT( ret == 1);
	
		lua_pushstring( L, "__index" );				// return a function closure with upvalue(2)=obj
		lua_pushcclosure( L, native_inst_index, 0 ); 
		lua_rawset( L, -3 );

		lua_pushstring( L, "__newindex" );
		lua_pushcclosure( L, native_inst_newindex, 0 ); 
		lua_rawset( L, -3 );

		lua_pushstring( L, "__gc" );				// call the release method
		lua_pushcclosure( L, native_inst_ref_gc, 0 );
		lua_rawset( L, -3 );

		lua_pushstring( L, "__tostring" );			// debug output
		lua_pushcclosure( L, lua_meta_tostring, 0 ); 
		lua_rawset( L, -3 );
	}
}

void native_inst_meta( lua_State *L ){
	luaL_getmetatable( L, "__meta_instance" );
	if( lua_isnil( L, -1 ) ){
		lua_pop( L, 1);

		int ret = luaL_newmetatable( L, "__meta_instance" );
		ASSERT( ret == 1 );
	
		lua_pushstring( L, "__newindex" );
		lua_pushcclosure( L, native_inst_newindex, 0 ); 
		lua_rawset( L, -3 );

		lua_pushstring( L, "__index" );				// return a function closure with upvalue(2)=obj
		lua_pushcclosure( L, native_inst_index, 0 ); 
		lua_rawset( L, -3 );

		lua_pushstring( L, "__tostring" );			// debug output
		lua_pushcclosure( L, lua_meta_tostring, 0 ); 
		lua_rawset( L, -3 );
	}
}

void lua_inst_meta( lua_State* L){
	luaL_getmetatable( L, "__meta_lua_inst" );
	if( lua_isnil( L, -1 ) ){
		lua_pop( L,1 );

		luaL_newmetatable( L, "__meta_lua_inst" );

		lua_pushstring( L, "__newindex" );			// assign an attribute to env table or just a table
		lua_pushcclosure( L, lua_inst_newindex, 0 ); 
		lua_rawset( L, -3 );

		lua_pushstring( L, "__index" );				// return a function closure with upvalue(2)=obj
		lua_pushcclosure( L, lua_inst_index, 0 );	// searching on self-scope, class-scope and native scope
		lua_rawset( L, -3 );

		lua_pushstring( L, "__tostring" );			// debug output
		lua_pushcclosure( L, lua_meta_tostring, 0 ); 
		lua_rawset( L, -3 );

		lua_pushstring( L, "__eq" );				// equal
		lua_pushcclosure( L, lua_meta_eq, 0 ); 
		lua_rawset( L, -3 );
	}
}

void lua_inst_puppet_meta( lua_State* L){
	luaL_getmetatable( L, "__meta_lua_inst_puppet" );
	if( lua_isnil( L, -1 ) ){
		lua_pop( L,1 );

		luaL_newmetatable( L, "__meta_lua_inst_puppet" );

		lua_pushstring( L, "__newindex" );			// assign an attribute to env table or just a table
		lua_pushcclosure( L, lua_inst_newindex, 0 ); 
		lua_rawset( L, -3 );

		lua_pushstring( L, "__index" );				// return a function closure with upvalue(2)=obj
		lua_pushcclosure( L, lua_inst_index, 0 );	// searching on self-scope, class-scope and native scope
		lua_rawset( L, -3 );

		lua_pushstring( L, "__gc" );				// destroy the object
		lua_pushcclosure( L, lua_inst_gc, 0 ); 
		lua_rawset( L, -3 );

		lua_pushstring( L, "__tostring" );			// debug output
		lua_pushcclosure( L, lua_meta_tostring, 0 ); 
		lua_rawset( L, -3 );

		lua_pushstring( L, "__eq" );				// equal
		lua_pushcclosure( L, lua_meta_eq, 0 ); 
		lua_rawset( L, -3 );
	}
}

void lua_state_meta(lua_State *L){
	luaL_getmetatable( L, "__meta_lua_state" );
	if( lua_isnil( L, -1 ) ){
		lua_pop( L,1 );

		luaL_newmetatable( L, "__meta_lua_state" );
		lua_pushstring( L, "__gc");					
		lua_pushcclosure( L, lua_state_gc, 0);
		lua_rawset( L, -3);

		lua_pushstring( L, "__tostring" );			// debug output
		lua_pushcclosure( L, lua_meta_tostring, 0 ); 
		lua_rawset( L, -3 );
	}
}

void lua_statedef_meta(lua_State *L){
	luaL_getmetatable( L, "__meta_lua_statedef" );
	if( lua_isnil( L, -1 ) ){
		lua_pop( L,1 );

		luaL_newmetatable( L, "__meta_lua_statedef" );
		lua_pushstring( L, "__call");					
		lua_pushcclosure( L, lua_statedef_call, 0);
		lua_rawset( L, -3);

		lua_pushstring( L, "__tostring" );			// debug output
		lua_pushcclosure( L, lua_meta_tostring, 0 ); 
		lua_rawset( L, -3 );
	}
}

void lua_class_meta( lua_State* L){
	luaL_getmetatable( L, "__meta_lua_class" );
	if( lua_isnil( L, -1 ) ){
		lua_pop( L,1 );

		luaL_newmetatable( L, "__meta_lua_class" );
		lua_pushstring( L, "__call");				//  schedule a constructor call
		lua_pushcclosure( L, lua_class_call, 0);
		lua_rawset( L, -3);

		lua_pushstring( L, "__tostring" );			// debug output
		lua_pushcclosure( L, lua_meta_tostring, 0 ); 
		lua_rawset( L, -3 );
	}
}

_CHAOS_END