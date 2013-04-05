/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_REFERENCEDCOUNT_H
#define	_CHAOS_REFERENCEDCOUNT_H

#include <cassert>

_CHAOS_BEGIN

/*
A reference counting object that will delete itself when counting down zero.

the schema is similar to obj-c. when objects are explicitly created using new
operator, you should then "release" it afterwards.
because there is an autorelease pool, objects, if created by a function, are
not needed to release if not retained. The creaction functor will "autorelease" it

NB, you should not explicitly delete the referenced object even if you create
them using new operator.

*/

// TODO: move to autorelease pool
enum AutoreleasePolicy{
	AP_SINGLE,		// released at the end of each frame
	AP_DOUBLE,		// released every second frame
	AP_STACKED,		// released after poping up the stack
	AP_GLOBAL,		// released when application ends
};

class CHAOS_API ReferencedCount{
private:
	int		mRefCount;

public:
	ReferencedCount() : mRefCount( 1 ) {};
	virtual ~ReferencedCount(){	};

	inline void retain(){ ++ mRefCount; };
	inline void release(){
		assert( mRefCount > 0 );
		
		if( -- mRefCount == 0 )
			delete this;
	}

	inline int refCount() const{ return mRefCount; };
};

_CHAOS_END

#endif

