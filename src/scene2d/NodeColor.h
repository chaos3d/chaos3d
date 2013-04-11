/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_NODECOLOR_H
#define	_CHAOS_NODECOLOR_H

#include "common/common.h"
#include "wm4-algebra/algebra.h"

_CHAOS_BEGIN

class Scene2DNode;

/*
* Definition ext:
*	color = {red, green, blue, alpha}
*
* @remark
*	value ranging from 0.0 to 1.0 
*
* i.e.
*	color = {1,1,1,1}
*/

class NodeColor{

public:
	class ColorApplier;

protected:
	Wm4::ColorRGBA	mColor;		// global color
	Wm4::ColorRGBA	mLocalColor;	// self color
	Scene2DNode* const	mNode;

public:
	NodeColor(Scene2DNode*);
//	NodeColor(Scene2DNode*, TypeLua const&);

	Wm4::ColorRGBA const& getColor() const {return mColor;}
	void setColor(Wm4::ColorRGBA const&);
	void setColor(float r, float g, float b, float a);
	void updateColor();

    /*
	template<class Applier, class TE>
	AnimationState*	animation( NumericAnimation<Wm4::ColorRGBA>* na ){
		typedef NumericAnimationState<Applier, Wm4::ColorRGBA, TE> NAS;
		NAS* as = new NAS(na, Applier(this));
		as->autorelease();
		return as;
	};*/

	Scene2DNode* getNode() const { return mNode; };
};

_CHAOS_END

#endif

