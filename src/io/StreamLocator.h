/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_STREAMLOCATOR_H
#define	_CHAOS_STREAMLOCATOR_H

#include "common/common.h"
#include <string>
#include <map>

_CHAOS_BEGIN

class DataStream;

// how to find a stream
class CHAOS_API StreamLocator{
public:
	StreamLocator(){};
	virtual ~StreamLocator(){};

    virtual int priority(){ return 0; };
    
	// check if it exists in the locator
	virtual bool exist(const char* path) = 0;

	// create a stream
	virtual DataStream*	createStream(const char* path) = 0;
};

_CHAOS_END

#endif

