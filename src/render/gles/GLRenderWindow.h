/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_GLRENDERWINDOW_H
#define	_CHAOS_GLRENDERWINDOW_H

#include "chaos_config.h"
#include "2d/RenderWindow.h"
#include "glwrap.h"
#include <windows.h>

_CHAOS_BEGIN

/*
RenderWindow for Windows
*/
class GLRenderWindow : public RenderWindow{
protected:
	EGLDisplay	mEglDisplay;
	EGLSurface	mEglWindow;
	EGLContext	mEglContext;
	EGLConfig	mEglConfig;
	HWND		mWnd;
	HDC			mHDC;

	bool		create(long width, long height);
	bool		create();
	bool		initContext();
	EGLConfig	setEGLConfiguration();

public:
	GLRenderWindow();
	~GLRenderWindow();

	virtual void resize(bool);
	virtual void bind();
	virtual void flush();
};

_CHAOS_END

#endif
