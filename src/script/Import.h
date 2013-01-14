/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef _CHAOS_IMPORT_H
#define _CHAOS_IMPORT_H

#include "chaos_config.h"
#include "core/core.h"
#include "core/RTTI.h"
#include "script/ScriptManager.h"

_CHAOS_BEGIN

/**
 * Import a variable bound to a symbol table
 *
 * usage:
 * In C/C++, Obj-C/C++
 *	Import("runner",runner)
 *		.function(play, &Runner::play)	// class method
 *		.function(selector, SELECTOR1(void,int)	// obj selector
 *
 * TODO:
 *	Import<Scene2DNode>()		// extend existing table
 *		.function(newAnim, &Scene2DNode::linearAnim)
 *
 */
class CHAOS_API Import{
protected:
	const char		*mName;
	Type			*mType;
	void			*mObj;

public:
	template<class C> Import():mType(typename C::__getType()){ 
	}

	explicit Import(const char* name, void* obj) : mName(name), mObj(obj){
		mType = Type::classType("no-name",new SymbolTable());
	}

	~Import(){
		ScriptManager::getInstance()->loadSymbol(mName, mObj, mType);
	};

	template<class F>
	inline Import& function(const char* name, F functor){
		mType->table->insert(Type::functionType(name, functionDeduce(functor)));
		return *this;
	}

	inline Import& function(const char* name, LuaFunctor* functor){
		mType->table->insert(Type::functionType(name, functor));
		return *this;
	}
};

_CHAOS_END

#endif


