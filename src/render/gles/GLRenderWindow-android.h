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

_CHAOS_BEGIN

/**
 * render window for android
 * it actually does nothing, it will be handled on Java-side
 * this is an interface to some values
 */
class GLRenderWindow : public RenderWindow{
protected:

public:
	GLRenderWindow();
	virtual ~GLRenderWindow();

	void setSize(float w, float h) {
		mSize = Rectanglef( Vector2f(0.f,0.f), w, h);
	}
	virtual void resize(bool) {};
	virtual void bind();
	virtual void flush();
};

_CHAOS_END

#endif
