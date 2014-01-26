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
#include <memory>
#include <type_traits>

#define SAFE_DELETE0(obj)   if( (obj) != 0 ) { delete (obj); (obj) = 0; }
#define SAFE_DELETE(obj)    if( (obj) != 0 ) { delete (obj); }

#define SAFE_RELEASE0(obj)  if( (obj) != 0 ) { (obj)->release(); (obj) = 0; }
#define SAFE_RELEASE(obj)   if( (obj) != 0 ) { (obj)->release(); }
#define SAFE_RETAIN(obj)    if( (obj) != 0 ) { (obj)->retain(); }
#define SAFE_ASSIGN(obj, o) do{ if( obj == o) break; if( (obj) != 0 ) { delete (obj); } (obj) = (o); } }while(0)
#define SAFE_REFAGN(obj, o) do{ if( obj == o) break; if( (obj) != 0 ) { (obj)->release(); } if(((obj) = (o)) != 0) { (obj)->retain(); } }while(0)
#define SAFE_AUTOREFAGN(obj, o) do{ if( obj == o) break; if( (obj) != 0 ) { (obj)->autorelease(); } if(((obj) = (o)) != 0) { (obj)->retain(); } }while(0)

#define RETAIN(obj)         ((obj)->retain(), obj)
#define AUTORELEASE(obj)    ((obj)->autorelease(), obj)

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

class referenced_count{
public:
    struct release_deleter {
        void operator()(referenced_count* obj) const {
            obj->release();
        }
    };
public:
	referenced_count() : _ref_count( 1 ) {};
	virtual ~referenced_count() {};

	void retain() {
	   	++ _ref_count; 
	};

	void release() {
		assert( _ref_count > 0 );
		
		if( -- _ref_count == 0 )
			delete this;
	}

	int ref_count() const { return _ref_count; };

private:
	int _ref_count;
};

#endif

