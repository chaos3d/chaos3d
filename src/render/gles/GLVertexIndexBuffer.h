/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_GLVERTEXINDEXBUFFER_H
#define	_CHAOS_GLVERTEXINDEXBUFFER_H

#include "chaos_config.h"
#include "../HardwareBuffer.h"
#include "glwrap.h"

_CHAOS_BEGIN

class GLVertexIndexBuffer : public HardwareBuffer{
protected:
	GLuint		mBufferId;

public:

	GLVertexIndexBuffer( size_t sizeInBytes, void* data, int usage );
	~GLVertexIndexBuffer();

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

class GLVertexIndexClientBuffer : public HardwareBuffer{
protected:
	char*	mBuffer;

public:

	GLVertexIndexClientBuffer( size_t sizeInBytes, void* data, int usage );
	~GLVertexIndexClientBuffer();

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

