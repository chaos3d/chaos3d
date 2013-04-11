#ifndef _SCENE2DROOT_H
#define _SCENE2DROOT_H

#include "common/common.h"

_CHAOS_BEGIN

class Camera;
class EventHandler;

class Scene2DRoot : public Scene2DNode {
public:
	/**
	 * more cameras can either be created by extending this class
	 * but it could be so common that you might need more than
	 * one cameras, and it won't cost too much, then put it in
	 * the base class
	 */
	enum {
		NUM_CAMERA = 4,	// fixed number of cameras
	};

	virtual ~Scene2DRoot();

    virtual void renderAllCamera();
    
protected:
	EventHandler* _handler;
	Camera*	_camera[NUM_CAMERA];	// camera as a component
};

_CHAOS_END

#endif
