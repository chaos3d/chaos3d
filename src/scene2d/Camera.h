#ifndef _CAMERA_H
#define _CAMERA_H

/// component naming conflicts could be resolved by adding a namespace scene2d

#include "common/common.h"
#include "wm4-algebra/algebra.h"
#include "re/RenderPass.h"	// TODO

_CHAOS_BEGIN

class Scene2DNode;

/**
 * Camera is a logic and graphical unit that combines
 * the objects in the scene and renders them into the
 * render target. it glues the gap between the logic
 * and the graphics. 
 *
 * tasks it might do:
 *  * renders sprites to the target
 *  * culls the sprites
 *  * transforms the position
 *  * selects the objects
 */

class Camera {
	typedef Wm4::ColorRGBA ColorRGBA;
	typedef Wm4::Matrix4f Matrix4f;

public:
	virtual ~Camera() {};

	void setClearColor(Wm4::ColorRGBA const& clear) { _clearColor = clear; }
	Wm4::ColorRGBA const& getClearColor() const { return _clearColor; }

	Matrix4f const& getProjectiveMatrix() const{ return _projMatrix; };
	Matrix4f const& getTransformMatrix() const;
	Matrix4f const& getTransformInvMatrix() const { return _transformInv;};

	virtual Scene2DNode* pick(Wm4::Vector2f const& pos, Wm4::Vector3f& q);
	void unproject(Wm4::Vector3f&);

	virtual void render();

private:
	Scene2DNode* _node;
	ColorRGBA _clearColor;
	Matrix4f _projMatrix;
	Matrix4f _projMatrixInv;
	Matrix4f _transformInv;

	RenderTarget* _target;
	Viewport _viewport;
};

_CHAOS_END

#endif
