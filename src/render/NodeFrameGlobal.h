/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_NODEFRAMEGLOBAL_H
#define	_CHAOS_NODEFRAMEGLOBAL_H

#include "chaos_config.h"
#include "NodeFrame.h"

_CHAOS_BEGIN

class NodeFrameGlobal : public NodeFrame{
public:
	NodeFrameGlobal(Scene2DNode *n) : NodeFrame(n){};
	NodeFrameGlobal(Scene2DNode *n, TypeLua const& lua) : NodeFrame(n,lua){};
	virtual Scene2DNode* pick( Vector3f const& p, Vector3f const& d, Vector3f& q, int& ret);
};

_CHAOS_END

#endif

