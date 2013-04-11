/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_AUTORELEASEPOOL_H
#define	_CHAOS_AUTORELEASEPOOL_H

#include "chaos_config.h"
#include <list>
#include <vector>
#include "ReferencedCount.h"
#include "Singleton.h"

_CHAOS_BEGIN

class ReferencedCount;

/*
Referenced object will be automatically released if put into the pool.

the pool itself could adopt different policies to handle the lifetime
e.g.
1. single frame buffered, where objects will be released after the frame
2. double-frame buffered, where objects will be released every seconde frame

*/
class CHAOS_API AutoreleasePool : public Singleton<AutoreleasePool>{
public:
	typedef std::list<ReferencedCount*>		TRefList;
	typedef std::vector<TRefList>			TPoolStack;

protected:
	TRefList		mGlobalPool;
	TPoolStack		mStackPool;
	TRefList		mDBBufferPool[2];
	int				mCurBuffer;

public:
	~AutoreleasePool();
	AutoreleasePool();

	void	autorelease(ReferencedCount*, int policy = AP_SINGLE);

	void	cleanUp();

	void	push();
	void	pop( int n );
};

_CHAOS_END

#endif

