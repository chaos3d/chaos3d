/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_TEXTURE_H
#define	_CHAOS_TEXTURE_H

#include "chaos_config.h"
#include "core/math.h"
#include "res/Resource.h"

_CHAOS_BEGIN

enum WrapMode{
	CLAMP_TO_EDGE,
	REPEAT,
};

enum FilterMode{
	LINEAR,
	NEAREST_MIPMAP_NEAREST,
	NEAREST,
	NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR,
};

/*
*/
DECLARE_CLASS(Texture, NilParent)
class Texture : public Resource{
public:
	enum{
		TEXTURE_1D,
		TEXTURE_2D,
		TEXTURE_3D,
	};

	enum{
		Format2D_RGBA8888,
		Format2D_RGB565,
		Format2D_A8,
		Format2D_RGB_PVRTC_4BPPV1,
		Format2D_RGB_PVRTC_2BPPV1,
		Format2D_RGBA_PVRTC_4BPPV1,
		Format2D_RGBA_PVRTC_2BPPV1,
	};

protected:
	int				mType;
	FilterMode		mMinFilter, mMagFilter;
	WrapMode		mWrapS, mWrapT;
	Vector2f		mSize;
	bool			mGenMipmap;		// generate mipmaps


public:
	Texture(int type = TEXTURE_2D, int minFilter = NEAREST_MIPMAP_NEAREST, int magFilter = LINEAR,
		int wrapS = CLAMP_TO_EDGE, int wrapT = CLAMP_TO_EDGE, bool genMipmap = true) 
		: mType(type), mMinFilter((FilterMode)minFilter),mMagFilter((FilterMode)magFilter), 
		mWrapS((WrapMode) wrapS ), mWrapT((WrapMode) wrapT),
		mSize( 0.f, 0.f), mGenMipmap( genMipmap )
	{
	};

	inline Vector2f const&	getSize() const{ return mSize; };

	inline int		getType() const{ return mType;	};

	virtual void	loadData(void* data, int width, int height, int level, int format, size_t size) = 0;
	virtual void	loadSubData(void* data,int x, int y, int width, int height, int format) = 0;
	virtual void	bind() {};
	virtual void	unBind() {};

};

_CHAOS_END

#endif