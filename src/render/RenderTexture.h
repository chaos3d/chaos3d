/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_RENDERTEXTURE_H
#define	_CHAOS_RENDERTEXTURE_H

#include "chaos_config.h"
#include "RenderTarget.h"
#include "Texture.h"

_CHAOS_BEGIN

class RenderTexture : public RenderTarget{
protected:
	Texture		*mTarget;

public:
	RenderTexture(Texture* tex) : mTarget(tex) {
		if( mTarget != 0 )
			mTarget->retain();
	};

	virtual ~RenderTexture() {
		if( mTarget != 0 )
			mTarget->release();
	}

	virtual void setTexture(Texture *tex){
		if( tex != 0 )
			tex->retain();
		if( mTarget != 0 )
			mTarget->release();
		mTarget = tex;
	}

	Texture*	getTexture() const { return mTarget;};

};

_CHAOS_END

#endif

