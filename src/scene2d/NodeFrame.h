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

#include "common/common.h"
#include "wm4-algebra/algebra.h"

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

class NodeFrame{
protected:
	Scene2DNode	*const _node;
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
	NodeFrame(Scene2DNode *n);
//	NodeFrame(Scene2DNode*, TypeLua const&);

	void setFrame(float x, float y, float hW, float hH );
	void setFrameFromeSprite();

	float const*	getCenter() const { return mCenter;};
	float const*	getExtent() const { return mExtent;};

    Scene2DNode* getNode() const { return _node; };
    
	// test if the ray intersects across the boundary
	// R(t) = p + t*d, where p and d are in the global space
	bool test(Wm4::Vector3f const& p, Wm4::Vector3f const& d) const;
	int	testIntersect(Wm4::Vector3f const& p, Wm4::Vector3f const& d, Wm4::Vector3f& q, Wm4::Vector2f const& extra = Wm4::Vector2f(0.f,0.f)) const;
	void intersect( Wm4::Vector3f const& p, Wm4::Vector3f const& d, Wm4::Vector3f& q) const;
	virtual Scene2DNode* pick( Wm4::Vector3f const& p, Wm4::Vector3f const& d, Wm4::Vector3f& q, int& ret);
};

_CHAOS_END

#endif
