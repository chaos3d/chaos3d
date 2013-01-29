/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_GLRENDERWINDOW_IOS_H
#define	_CHAOS_GLRENDERWINDOW_IOS_H

#include "chaos_config.h"
#include "2d/RenderWindow.h"
#include "2d/gles/GLRender2DManager-ios.h"
#include "glwrap.h"

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import <QuartzCore/QuartzCore.h>

@class EAGLView;

_CHAOS_BEGIN

/**
 * RenderWindow for iPhone
 * Context is only used for rendering, so resources, like texture
 * maybe created in the other context? so when this one is released
 * the resources still exist
 */
class GLRenderWindow : public RenderWindow{
protected:
	EAGLView	*mView;
	EAGLContext	*mContext;
	GLuint	mRenderBufferID, mDepthBufferID;
	GLuint	mFrameBufferID;

	//TouchEvent	mTouches;
	
	void		create();
	
public:
	GLRenderWindow(EAGLView*, EAGLContext*);
	~GLRenderWindow();
	
	EAGLView* getView() const { return mView; };
	
	virtual void resize(bool anim);
	virtual void bind();
	virtual void flush();
};

_CHAOS_END

/*
 This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
 The view content is basically an EAGL surface you render your OpenGL scene into.
 Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
 
 TODO: The touch events should be pushed to event queue with the target of the view itself
 */
@interface EAGLView : UIView {
	GLRenderWindow*	window;
	CGAffineTransform	ptTransform;
}

@property (nonatomic, assign) CGAffineTransform	ptTransform;
- (void) setWindow:(GLRenderWindow*) w;
- (void) initLayer;

@end

#endif
