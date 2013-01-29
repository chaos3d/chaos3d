/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_GLRENDERTEXTUREES1_H
#define	_CHAOS_GLRENDERTEXTUREES1_H

#include "chaos_config.h"
#include "../RenderTexture.h"
#include "GLTexture.h"
#include "glwrap.h"

_CHAOS_BEGIN

class GLRenderTextureES1 : public RenderTexture{
protected:
	EGLSurface mPBufferSurface[2];
	EGLDisplay mCurrentDisplay;
	EGLContext mCurrentContext;
	EGLSurface mCurrentSurface;

	int		mCurrentBuffer;

	EGLConfig selectConfig();
	void	destroy();
public:
	GLRenderTextureES1(GLTexture* tex);
	virtual ~GLRenderTextureES1();

	virtual void setTexture(Texture*);
	virtual void bind();
	virtual void flush();
};

_CHAOS_END

#endif
