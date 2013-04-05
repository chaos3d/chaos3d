// ----------------------------------------------
//
// SceneVisitor.h
//
// Created by wuh (3/24)
// Copyright (c) 2009
//
// ----------------------------------------------


#ifndef	_CHAOS_SCENEVISITOR_H
#define	_CHAOS_SCENEVISITOR_H

namespace chaos{

	class SceneNode;
	class SceneVisitor{
	public:
		// true if visit children, false otherwise culled
		virtual bool	visit(SceneNode* ) = 0;
	};
};

#endif
