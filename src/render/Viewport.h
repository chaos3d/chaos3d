// ----------------------------------------------
//
// Viewport.h
//
// Created by wuh (2/20)
// Copyright (c) 2009
//
// ----------------------------------------------


#ifndef	_CHAOS_VIEWPORT_H
#define	_CHAOS_VIEWPORT_H

#include "core/math.h"
#include "core/SharedPtr.h"
#include "Renderable.h"
#include "script/bind.h"

#include <list>
#include <vector>

_CHAOS_BEGIN

struct Renderable;

/*
To provide specific renderables to viewport/camera
*/
class Viewport;
class Renderer{
public:
	typedef SharedPtr<Renderer>	Ptr;		
	virtual void	doRender(Viewport*) = 0;
	virtual ~Renderer(){};
};

/*
A region on the target. viewport is created and held by a target that applied to.

It's common to move around the 3d-space or a 2d-scroll game. This can be done by
the camera, who holds the viewport and change its matrix as needed.

Viewport can be seen as a 2d/3d canvas where objects will be rendered to.
*/
// should change name to camera
class Viewport{
public:
	enum ClearTypeMask{
		Color = 0x1,
		Depth = 0x2
	};

protected:
	cRect			mViewport;
	Matrix4f		mProjMatrix;
	Matrix4f		mProjMatrixInv;

	/*
	the only different between them is sorting criteria, that are
	Opaque objects are material-first, and z-order ascending (not needed in the defered render algo.)
	Transparent objects are z-order descending first and material.
	*/
	typedef std::list<Renderable>	TRenderableList;
	TRenderableList			mOpaqueRenderables;
	TRenderableList			mTransparentRenderables;

	typedef std::vector<Renderer::Ptr>	TRendererList;
	TRendererList			mRenderers;

public:
	virtual ~Viewport();

	void	setViewport(cRect const& viewport){
		mViewport = viewport;
	};

	//set projection matrix directly
	void		setProjection( Matrix4f const& proj ){
		mProjMatrix = proj;
	};

	Matrix4f	getProjection( ) const{
		return mProjMatrix;
	};

	virtual void	unProject(Vector3f& viewPos) const;

	/*
	set up the project matrix according to different
	coordinate system used by different render api
	*/
	virtual void	setPerspectiveProj(const float left,const float bottom,const float right,const float top,const float near,const float far);
	virtual void	setOrthoProj(const float left,const float bottom,const float right,const float top,const float near,const float far);

	/*
	help set up projection easily
	*/
	/*
	(-1,-1) mapped to (-1,-1);
	fov == arctan(right/near)*2 in radian, a.k.a the angle of view along y-axis
	aspect == width/height == right/top
	*/
	void	setPerpective(const float fov,const float aspect,const float near,const float far);

	/*
	(0,0) is mapped to (-1,-1)
	*/
	void	setOrtho(const float width,const float height,const float near,const float far);

	/*
	subjected to change
	*/
	//void	addOpaqueRenderable(Material*, VertexBuffer*, VertexIndexBuffer*, RenderOperation::OperationType);
	//void	addTransparentRenderable(Material*, VertexBuffer*, VertexIndexBuffer*, RenderOperation::OperationType);

	void	addTransparentRenderable(Renderable const& rd);

	/*
	render all objects into viewport whose target is the current used one,
	which is bounded by its calling method.
	*/
	virtual void	doRender();

	void	addRenderer(Renderer::Ptr);

	/*
	clear the viewport
	*/
	virtual void	clear(int type, float* color) = 0;

	//DECLARE_CLASS( Viewport );
};

_CHAOS_END

#endif
