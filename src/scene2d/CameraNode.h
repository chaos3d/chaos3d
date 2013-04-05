/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_CAMERANODE_H
#define	_CHAOS_CAMERANODE_H

#include "chaos_config.h"
#include "Scene2DNode.h"

#include "core/math.h"
#include "Transform.h"

_CHAOS_BEGIN

DECLARE_CLASS(CameraNode, Scene2DNode);
class CameraNode : public Scene2DNode{
	DYNAMIC_CLASS(CameraNode);

protected:
	Matrix4f	mProjMatrix;
	Matrix4f	mProjMatrixInv;
	Matrix4f	mTransformInv;

public:
	CameraNode(std::string const& tag)
		:Scene2DNode(tag),
		mProjMatrix(Matrix4f::IDENTITY),
		mProjMatrixInv(Matrix4f::IDENTITY),
		mTransformInv(Matrix4f::IDENTITY)
	{};

	virtual ~CameraNode();

	virtual void unproject(Vector3f& viewPos, cRect const& vp) const = 0;
	virtual void update();

	Matrix4f const& getProjectiveMatrix() const{ return mProjMatrix; };
	Matrix4f const& getTransformMatrix() const;
	Matrix4f const& getTransformInvMatrix() const { return mTransformInv;};
};

TYPE_RET_REF(CameraNode);

_CHAOS_END

#endif

