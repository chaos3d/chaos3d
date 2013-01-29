/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_RENDERTARGET_H
#define	_CHAOS_RENDERTARGET_H

#include "chaos_config.h"
#include "core/core.h"
#include "core/math.h"

_CHAOS_BEGIN

class RenderTarget : public ReferencedCount{
protected:
	Rectanglef	mSize;
	
public:
	Rectanglef const& getSize() const {return mSize;};
	//Rectanglef& size() {return mSize;}
	
	virtual void bind() = 0;
	virtual void flush() = 0;
};

_CHAOS_END

#endif
