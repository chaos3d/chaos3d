/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_SCRIPTDATA_H
#define	_CHAOS_SCRIPTDATA_H

#include "chaos_config.h"

_CHAOS_BEGIN

/*
extensions for lua
*/
class Type;
class GOClass;
class GOInstance;
class GOState;

/*
A script type of userdata in lua
*/
struct ScriptData{
	union{
		//Instance
		struct{
			Type*		type;
			void*		obj;
		};

		//Class
		struct{
			Type*		class_;
		};

		//State
		struct{
			GOState*	state;
		};

		//GOClass
		struct{
			GOClass*	go_class;
		};

		//GOInst
		struct{
			GOClass*	go_type;	// go class type
			GOInstance*	go_inst;	// object pointer
		};
	};


	enum{
		Class,			// class/function/property definition
		Instance,		// instance variable
		This,			// this pointer
		State,			// state type
		StateDef,		// state definition
		GOClass,		// go class definition
		GOInst,			// game object
	}	dataType;

	static const char* typeName(int);
};

_CHAOS_END

#endif

