/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_GLRENDERTEXTURE_H
#define	_CHAOS_GLRENDERTEXTURE_H

#include "chaos_config.h"
#include "../RenderTexture.h"
#include "glwrap.h"

_CHAOS_BEGIN

class GLTexture;

class GLRenderTexture : public RenderTexture{

protected:
	GLuint	mFrameID;

	void	destroy();
public:
	GLRenderTexture(GLTexture*);
	virtual ~GLRenderTexture();

	virtual void setTexture(Texture*);
	virtual void bind();
	virtual void flush();
};

_CHAOS_END

#endif

