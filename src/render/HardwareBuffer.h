/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_HARDWAREBUFFER_H
#define	_CHAOS_HARDWAREBUFFER_H

#include "chaos_config.h"
#include "core/core.h"

_CHAOS_BEGIN

/*
A buffer where to store client data, such as vertex and index.
*/
class HardwareBuffer : public ReferencedCount{

protected:
	size_t				mSizeInBytes;
	bool				mIsBind;
	bool				mIsLocked;

	/*
	Only created by render manager
	*/
	HardwareBuffer(size_t sizeInBytes) : mSizeInBytes(sizeInBytes){
	};

public:
	// lock to retreive the address to write
	virtual void*		lock() = 0;

	// unlock the retrieved address
	virtual void		unLock() = 0;

	// get the pointer of the offset to its origin
	virtual void*		getOffset( int offset ) = 0;

	// bind the buffer
	virtual void		bind() = 0;

	// unbind the buffer
	virtual void		unBind() = 0;

	virtual ~HardwareBuffer() {};

};

_CHAOS_END

#endif
