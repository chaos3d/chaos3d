/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_NODEFRAME_H
#define	_CHAOS_NODEFRAME_H

#include "chaos_config.h"
#include "core/math.h"
#include "core/RTTI.h"
#include "script/LuaType.h"

_CHAOS_BEGIN

class Scene2DNode;

/*
* Definition ext:
*	frame = {origin x,origin y,half width, half height}|true|false
*
* @remark
*	if frame not specified, then
*		if neither ui nor sprite existed, default global frame is applied,
*		which will test each child but not itself
*		otherwise, frame generated from its sprite bound
*	if true, then
*		if sprite existed, frame generated from its sprite bound
*		otherwise, global frame is applied
*	if false, then frame is nil
*	otherwise a rectangle, indicating the actual frame
*	
*	In addition, some ui components may generate frame automatically. In this
*	case, if frame not specified, rules won't be applied. Others are the same.
*
* i.e.
*	frame = {0,0,160,240}
*/

DECLARE_CLASS(NodeFrame, NilParent);

class NodeFrame{
protected:
	Scene2DNode	*mNode;
	float	mCenter[2];
	float	mExtent[2];

	/*
	struct{
		unsigned int*	data;
		unsigned short	width, height;	// width will be aligned with 32-bit but filled 0 in double sides so data are centered.
		unsigned short	unit, capacity;
	}			mMask;
	*/
	// todo, add bounding volume

public:
	NodeFrame(Scene2DNode *n) : mNode(n){};
	NodeFrame(Scene2DNode*, TypeLua const&);

	void setFrame(float x, float y, float hW, float hH );
	void setFrameFromeSprite();

	float const*	getCenter() const { return mCenter;};
	float const*	getExtent() const { return mExtent;};

	// test if the ray intersects across the boundary
	// R(t) = p + t*d, where p and d are in the global space
	bool test(Vector3f const& p, Vector3f const& d) const;
	int	testIntersect( Vector3f const& p, Vector3f const& d, Vector3f& q, Vector2f const& extra = Vector2f(0.f,0.f)) const;
	void intersect( Vector3f const& p, Vector3f const& d, Vector3f& q) const;
	virtual Scene2DNode* pick( Vector3f const& p, Vector3f const& d, Vector3f& q, int& ret);
};

_CHAOS_END

#endif
