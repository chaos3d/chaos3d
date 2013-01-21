/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef _CHAOS_SCRIPT_DISPATCHER_H
#define _CHAOS_SCRIPT_DISPATCHER_H

#include "common.h"

struct lua_State;

_CHAOS_BEGIN

// lua extensions
extern CHAOS_PRIVATE int script_class(lua_State* L);
extern CHAOS_PRIVATE int script_state(lua_State* L);


///---------------------------------------
// native class and instance support
extern CHAOS_PRIVATE void native_class_meta( lua_State *L );
extern CHAOS_PRIVATE void native_inst_meta( lua_State *L );
extern CHAOS_PRIVATE void native_instgc_meta( lua_State *L );
extern CHAOS_PRIVATE void native_instref_meta( lua_State *L );

// native functor and object(if needed) are saved in the upvalue
// call them directly and push the result back to stack
extern CHAOS_PRIVATE int native_function_call( lua_State *L );

extern CHAOS_PRIVATE int native_inst_index( lua_State *L );
extern CHAOS_PRIVATE int native_inst_newindex( lua_State *L );
extern CHAOS_PRIVATE int native_inst_ref_gc( lua_State *L );
extern CHAOS_PRIVATE int native_inst_gc( lua_State *L );

extern CHAOS_PRIVATE int native_class_index( lua_State *L );
extern CHAOS_PRIVATE int native_class_call( lua_State *L );


///---------------------------------------
// lua class and instance support
extern CHAOS_PRIVATE void lua_class_meta( lua_State* L);
extern CHAOS_PRIVATE void lua_inst_meta( lua_State* L );
extern CHAOS_PRIVATE void lua_inst_puppet_meta( lua_State* L);

// instance meta method
extern CHAOS_PRIVATE int lua_inst_index(lua_State* L);
extern CHAOS_PRIVATE int lua_inst_newindex(lua_State* L);
extern CHAOS_PRIVATE int lua_inst_gc(lua_State* L);

// function call
extern CHAOS_PRIVATE int lua_function_call(lua_State* L);

// declare a go class
extern CHAOS_PRIVATE int lua_class(lua_State* L);
extern CHAOS_PRIVATE int lua_class_index(lua_State* L);		// for derived classes, not supported yet, todo list
extern CHAOS_PRIVATE int lua_class_call(lua_State* L);


///---------------------------------------
// lua state support
extern CHAOS_PRIVATE void lua_state_meta( lua_State* L);
extern CHAOS_PRIVATE void lua_statedef_meta( lua_State* L);
extern CHAOS_PRIVATE int lua_statedef_call( lua_State* L);
extern CHAOS_PRIVATE int lua_state_gc( lua_State* L);

///---------------------------------------
// debug support
extern CHAOS_PRIVATE int lua_meta_tostring( lua_State* L);
extern CHAOS_PRIVATE int lua_meta_eq(lua_State* L);

_CHAOS_END;

#endif