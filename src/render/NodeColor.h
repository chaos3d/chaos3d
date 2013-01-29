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

#include "chaos_config.h"
#include "core/math.h"
#include "core/RTTI.h"
#include "script/LuaType.h"

_CHAOS_BEGIN

class Scene2DNode;
class AnimationState;
template<class, class, class> class NumericAnimationState;
template<class> class NumericAnimation;

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

DECLARE_CLASS(NodeColor, NilParent);
class NodeColor{

public:
	class ColorApplier;

protected:
	ColorRGBA	mColor;		// global color
	ColorRGBA	mLocalColor;	// self color
	Scene2DNode* const	mNode;

public:
	NodeColor(Scene2DNode*);
	NodeColor(Scene2DNode*, TypeLua const&);

	ColorRGBA const& getColor() const {return mColor;}
	void setColor(ColorRGBA const&);
	void setColor(float r, float g, float b, float a);
	void updateColor();

	template<class Applier, class TE>
	AnimationState*	animation( NumericAnimation<ColorRGBA>* na ){
		typedef NumericAnimationState<Applier, ColorRGBA, TE> NAS;
		NAS* as = new NAS(na, Applier(this));
		as->autorelease();
		return as;
	};

	Scene2DNode* getNode() const { return mNode; };
};

_CHAOS_END

#endif

