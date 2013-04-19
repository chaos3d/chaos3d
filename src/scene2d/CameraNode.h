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

#include "common/common.h"
#include "Scene2DNode.h"

_CHAOS_BEGIN

class Camera;

class CameraNode : public Scene2DNode{
public:
	CameraNode(std::string const& tag)
		:Scene2DNode(tag)
	{};

	virtual ~CameraNode();

    //void createCamera();
    void setCamera(Camera*);
    Camera* getCamera() const { return _camera; }
    
    virtual void doRender();
    
#if 0
protected:
	virtual void unproject(Vector3f& viewPos, cRect const& vp) const = 0;
	virtual void update();
    
	Matrix4f const& getProjectiveMatrix() const{ return mProjMatrix; };
	Matrix4f const& getTransformMatrix() const;
	Matrix4f const& getTransformInvMatrix() const { return mTransformInv;};
    
	Matrix4f	mProjMatrix;
	Matrix4f	mProjMatrixInv;
	Matrix4f	mTransformInv;
#endif
    
protected:
    Camera* _camera;
};

_CHAOS_END

#endif

