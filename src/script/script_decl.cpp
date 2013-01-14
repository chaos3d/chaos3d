/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "script/dispatcher.h"
#include "script/scripting.h"
#include "script/bind.h"

#include "core/core.h"
#include "core/RTTI.h"
#include "go/go.h"

_CHAOS_BEGIN

// state("name") {}
// first return a userdata with a metatable of __call
// then initializing the state and return.
// TODO
//  or state("name", {})
int script_state(lua_State* L){
	luaL_argcheck( L, lua_isstring(L, 1), 1, "expected a state name.");
	luaL_argcheck( L, strlen(lua_tostring(L, 1)) > 0, 1, "state name cannot be \"\".");

	ScriptData *data = (ScriptData*)lua_newuserdata( L, sizeof(ScriptData) );
	data->dataType = ScriptData::StateDef;

	lua_statedef_meta(L);
	lua_setmetatable( L, -2);

	lua_newtable(L);
	lua_pushvalue( L, 1);
	lua_rawseti( L, -2, 1);

	lua_setfenv( L, -2 );

	return 1;
}

int script_class(lua_State* L){
	int argn = lua_gettop(L);
	lua_State *mainL(ScriptManager::getInstance()->getState());

	// class name or class definition
	if( argn == 1 ){
		if( !lua_isnone( L, lua_upvalueindex(2) ) )			// already define the parent class
			luaL_error(L, "Too many class names (%s), or parenthesize the parent list.", lua_tostring(L,1));
		else if( !lua_istable( L, 1) && !lua_isnone( L, lua_upvalueindex(1) ) ){		// one argument for parent class
			lua_pushvalue( L, lua_upvalueindex(1));
			lua_pushvalue( L, 1 );
			lua_pushcclosure( L, script_class, 2 );
		}else if( lua_isstring( L, 1 ) || lua_isnil( L,1 )){// class name or anonymous class
			lua_pushcclosure( L, script_class, 1);
		}else if( !lua_istable( L, 1) )						// already define class name and parent list, need a table
			luaL_error( L, "Expected class name or class definition. (%s)", luaL_typename(L,1) );
		else{
			// followed by a list of parent classes in upvalue
			// for now, just ignore them


			lua_newtable(L);

			lua_pushvalue( L, 1);
			lua_pushnil( L );

			while( lua_next( L, -2 ) ){
				if( lua_isfunction( L, -1 ) ){			// class method or constructor
					int key = (int)lua_tonumber( L, -2 );
					if( (key&(~1)) != 0 ){				// key != 0 && key != 1 
						LOG( "None name function has been ignored. May define the constructor or index not 1");
						lua_pop( L, 1);
					}else{
						lua_pushvalue( L, -2 );			//# table, class table, key, value, key
						lua_rawget( L, -5 );			//# table, class table, key, value, nil
						if( !lua_isnil( L, -1 ))
							LOG("Key %s already defined, replaced with a function", lua_tostring(L, -3));
						lua_pop( L, 1);
						lua_pushvalue( L, -2 );			//# table, class table, key, value, key
						lua_insert( L, -2 );			//# table, class table, key, key, value
						lua_rawset( L, -5 );
					}
				}
				else if( lua_isuserdata( L, -1 ) ){	// state("") {}
					if( lua_type( L, -2 ) != LUA_TNUMBER ){
						LOG( "State assigned to (%s), ignored the key", luaL_typename( L, -2)  );
					}
					ScriptData *data = (ScriptData*)lua_touserdata( L,-1);
					if( data->dataType == ScriptData::StateDef ){
						lua_getfenv( L, -1 );			//# table, class table, key, value, env
						lua_rawgeti( L, -1, 1);			//# table, class table, key, value, env, name
						lua_pushvalue( L, -1 );			//# table, class table, key, value, env, name, name
						lua_rawget( L, -7);				//# table, class table, key, value, env, name, nil
						if( !lua_isnil( L, -1 ))
							LOG("Key %s already defined, replaced with a state", lua_tostring(L, -2));
						lua_pop(L,1);
						
						ScriptData* stateData = (ScriptData*)lua_newuserdata(L, sizeof(ScriptData));
						stateData->dataType = ScriptData::State;

						lua_rawgeti( L, -3, 2 );		//# table, class table, key, value, env, name, state, new value(state table)
						lua_xmove(L, mainL, 1);			//move to main thread avoid of child thread's disposal
						stateData->state = new GOState(TypeLua(lua_ref(mainL,1)));	// TODO: native derived state classes
						lua_state_meta(L);
						lua_setmetatable(L, -2);		//# table, class table, key, value, env, name, state

						lua_rawset( L, -7 );			//# table, class table, key, value, env
						lua_pop( L, 2);
					}else{
						LOG( "Unable to assign the key/pair to the state table. ignored" );
						lua_pop( L, 1 );
					}

				}else{
					LOG( "Unable to assign the key/pair to the state table. ignored" );
					lua_pop( L,1 );
				}
			};	// # table, class table

			lua_pushvalue(L,-2);
			char const* className = 0;

			if( lua_isstring( L, lua_upvalueindex(1) )){
				className = lua_tostring(L, lua_upvalueindex(1));
			}

			// to save in the main thread in case child thread is disposed.
			lua_xmove(L, mainL, 1);
			//GOClass* goclass = GOClass::newClass(className, TypeLua(lua_ref(mainL,1)), GOInstance::__getType());
			GOClass* goclass = new GOClass(TypeLua(L), TYPE(GOInstance));
			
			ScriptData *data = (ScriptData*)lua_newuserdata( L, sizeof(ScriptData));
			data->dataType = ScriptData::GOClass;
			data->go_class = goclass;

			lua_class_meta(L);
			lua_setmetatable( L, -2);

			if( className != 0 ){
				lua_pushvalue( L, -1 );
				lua_setglobal( L, className);
			}
		}
	}
	
	else if( lua_isnone( L, lua_upvalueindex(1))){			// anonymous class name but with parent list
		lua_pushnil( L );									// insert an anonymous class name and push the closure
		lua_insert( L, 1);
		lua_pushcclosure( L, script_class, argn+1 );
	}
	
	else if( lua_isnone( L, lua_upvalueindex(2))){			// define parent classes list
		lua_pushvalue( L,lua_upvalueindex(1));				// push the class name
		lua_insert( L, 1);									// insert the name into the bottom
		lua_pushcclosure( L, script_class, argn+1 );
	}
	
	else													// maybe more than two lists of parent definitions
		luaL_error( L, "Too many arguments. (%d)", argn);

	return 1;
}

_CHAOS_END