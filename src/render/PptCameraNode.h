/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_PPTCAMERANODE_H
#define	_CHAOS_PPTCAMERANODE_H

#include "chaos_config.h"
#include "CameraNode.h"
#include "script/LuaType.h"

_CHAOS_BEGIN

/*
Camera using perspective projection matrix
*/
DECLARE_CLASS(PptCameraNode, CameraNode);
class PptCameraNode : public CameraNode{

public:
	PptCameraNode(std::string const& tag) : CameraNode(tag){};
	PptCameraNode(TypeLua const&);
	void setPerspectiveProj(const float left,const float bottom,const float right,const float top,const float near,const float far);
	void setPerpective(const float fov,const float aspect,const float near,const float far);

	virtual void unproject(Vector3f& viewPos, cRect const& vp) const;
	virtual void parseLua(TypeLua const&);
};

TYPE_RET_REF(PptCameraNode);

_CHAOS_END

#endif

