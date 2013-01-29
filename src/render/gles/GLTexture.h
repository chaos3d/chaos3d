/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_GLTEXTURE_H
#define	_CHAOS_GLTEXTURE_H

#include "chaos_config.h"
#include "../Texture.h"
#include "glwrap.h"

_CHAOS_BEGIN


class GLTexture : public Texture{
protected:
	GLuint		mTexId;

protected:
	void	setParameters();

public:
	static const GLenum wrapMap[2];
	static const GLenum filterMap[5];

	GLTexture(int type = TEXTURE_2D, int minFilter = NEAREST_MIPMAP_NEAREST, int magFilter = LINEAR,
		int wrapS = CLAMP_TO_EDGE, int wrapT = CLAMP_TO_EDGE, bool genMipmap = true);

	virtual ~GLTexture();

	virtual void	loadData(void* data, int width, int height, int level, int format, size_t size);
	virtual void	loadSubData(void* data,int x, int y, int width, int height, int format);
	virtual void	bind();
	
	inline GLuint	texId() const { return mTexId; };
	//virtual void	unBind();
};

_CHAOS_END

#endif
