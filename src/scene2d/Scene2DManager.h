/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_SCENE2DMANAGER_H
#define	_CHAOS_SCENE2DMANAGER_H

#include "chaos_config.h"
#include "event/evt.h"
#include "core/math.h"
#include "2d/render.h"

_CHAOS_BEGIN

class Scene2DNode;
class CameraNode;

DECLARE_CLASS(Scene2DManager, NilParent)
class Scene2DManager : public Singleton<Scene2DManager>{
	//DYNAMIC_CLASS;

protected:
	CameraNode		*mCamera;
	Scene2DNode		*mRootNode;
	RenderWindow	*mMainWindow;
	ColorRGBA		mClearColor;
	cRect			mVP;

	EventHandler	*mHandler;

	// handler
	/*
	void touchBegan(EventTouchBegan&);
	void touchMoved(EventTouchMoved&);
	void touchEnded(EventTouchEnded&);
	void touchCancelled(EventTouchCancelled&);
*/
public:
	Scene2DManager();
	virtual ~Scene2DManager();

	EventHandler&	getHandlerRef() { return mHandler ? *mHandler : EventNilHandler::NIL; }
	EventHandler*	getHandler() const { return mHandler; }
	void setHandler(EventHandler* handler) { SAFE_REFAGN(mHandler, handler); }

	RenderWindow*	getMainWnd() const{ return mMainWindow; };
	Scene2DNode*	getRoot() const { return mRootNode; };

	void	setCamera( CameraNode* );
	CameraNode* getCamera( ) const { return mCamera; };

	void	setClear( float r, float g, float b, float a);
	
	template<class F>
	void visit(F const&);

	virtual void update();
	virtual void renderScene();

    // it should be a converter tunnel that convert lua object to native object in a separate class 
	virtual Scene2DNode* createNode( TypeLua const& lua );
	virtual Scene2DNode* createNode( char const* name, char const* type );

	// pick a node
	virtual Scene2DNode* pick( Vector2f const& pos, Vector3f& q, int& r);
	void unproject( Vector3f &);

	// adjust viewport automatically due to device and orientation
	void adjustViewport();
	
	/**
	* update layout
	*/
	virtual void orientationChanged( );

};

//TYPE_RET_DYN(Scene2DManager);

_CHAOS_END

#endif
