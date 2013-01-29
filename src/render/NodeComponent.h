/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_NODECOMPONENT_H
#define	_CHAOS_NODECOMPONENT_H

#include "chaos_config.h"
#include "core/RTTI.h"
#include "script/LuaFactory.h"

struct lua_State;

#define	NODEFACTORY_OP		("__nodefactory")
#define IMPLEMENT_NODE(facClass)	\
	chaos::Type::constructorType( new facClass() ),

_CHAOS_BEGIN

class Scene2DNode;

DECLARE_CLASS(NodeComponent, NilParent);
class NodeComponent{
	//DYNAMIC_CLASS;
protected:
	Scene2DNode* const	mNode;

	virtual void parseElement(lua_State* L){
	};

public:
	virtual ~NodeComponent(){};
	NodeComponent(Scene2DNode* n) : mNode(n){
		ASSERT(mNode != 0 );
	};

	Scene2DNode* getNode() const{ return mNode; }
};

class NodeFactory : public LuaObjectFactory{
public:
	virtual ~NodeFactory(){}
	virtual NodeComponent*	createNodeObject(Scene2DNode*) = 0;

	virtual void	destroyLuaObject( void* obj ){};
	virtual	void*	createLuaObject( lua_State* L) { return 0; };
};

template<class N>
class GenericNodeFactory : public NodeFactory{
public:
	virtual NodeComponent*	createNodeObject(Scene2DNode* n){
		return new N(n);
	};
};


_CHAOS_END

#endif

