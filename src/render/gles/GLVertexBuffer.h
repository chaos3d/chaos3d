/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_GLVERTEXBUFFER_H
#define	_CHAOS_GLVERTEXBUFFER_H

#include "chaos_config.h"
#include "../HardwareBuffer.h"
#include "glwrap.h"

_CHAOS_BEGIN

class GLVertexBuffer : public HardwareBuffer{
protected:
	GLuint		mBufferId;

public:

	GLVertexBuffer( size_t sizeInBytes, void* data, int usage );
	~GLVertexBuffer();

	// lock to retreive the address to write
	virtual void*		lock();

	// unlock the retrieved address
	virtual void		unLock();

	// get the pointer of the offset to its origin
	virtual void*		getOffset( int offset );

	// bind the buffer
	virtual void		bind();

	// unbind the buffer
	virtual void		unBind();
};


class GLVertexClientBuffer : public HardwareBuffer{
protected:
	char*	mBuffer;

public:

	GLVertexClientBuffer( size_t sizeInBytes, void* data, int usage );
	~GLVertexClientBuffer();

	// lock to retreive the address to write
	virtual void*		lock();

	// unlock the retrieved address
	virtual void		unLock();

	// get the pointer of the offset to its origin
	virtual void*		getOffset( int offset );

	// bind the buffer
	virtual void		bind();

	// unbind the buffer
	virtual void		unBind();
};

_CHAOS_END

#endif

