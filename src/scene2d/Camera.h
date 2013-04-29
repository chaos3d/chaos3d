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
	typedef Wm4::Vector3f Vector3f;

public:
    Camera(Scene2DNode* node);
	virtual ~Camera();

    void setRootNode(Scene2DNode*);
    Scene2DNode* getRootNode() const { return _rootNode == NULL? _node : _rootNode; }
    
    Scene2DNode* getNode() const { return _node; }
    
	void setClearColor(Wm4::ColorRGBA const& clear) { _clearColor = clear; }
	Wm4::ColorRGBA const& getClearColor() const { return _clearColor; }

    void setTransformationInvMatrix(Matrix4f const& inv) { _transformInv = inv; }
	Matrix4f const& getTransformInvMatrix() const { return _transformInv;};

	virtual Scene2DNode* pick(Wm4::Vector2f const& pos, Wm4::Vector3f& q);
    
    // translate the position in the viewport (z=[0,-1) to the world (near, far)
	void viewportToWorld(Wm4::Vector3f&, Viewport const& vp) const;

    // orthographic projection matrix
    void setOrthoProj(const float left,const float bottom,const float right,const float top,
                      const float near,const float far);
    void setOrtho(const float width,const float height,const float near,const float far);
    
    // perspective projection matrix
    void setPerspectiveProj(const float left,const float bottom,const float right,const float top,
                            const float near,const float far);
	void setPerspective(const float fov,const float aspect,const float near,const float far);

    // projection matrix
	Matrix4f const& getProjectiveMatrix() const{ return _projMatrix; };
    
	virtual void render();

private:
	Scene2DNode* _node;
    // the node to start rendering; use _node if this is null (to prevent trivial circular
    // WARN: if the root is the parent node, it will be causing a circular reference
    //   make sure you reset to nil at some point before you lose it
	Scene2DNode* _rootNode;
	ColorRGBA _clearColor;
	Matrix4f _projMatrix;
	Matrix4f _projMatrixInv;
	Matrix4f _transformInv;

	RenderTarget* _target;
	Viewport _viewport;
};

_CHAOS_END

#endif
