/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_RESOURCELOADER_H
#define	_CHAOS_RESOURCELOADER_H

#include "chaos_config.h"
#include "core/SharedPtr.h"

_CHAOS_BEGIN
	
class Resource;
class DataStream;

/*
To decode data from the data stream and generate the holder objects.

Every type of data is denoted as a four-bytes of chars, such as 'jpeg' or 'png\0'.
A type of data is bound to a certain data loader and registered into loader data-
base.

A certain loader should know how to organize the data in the memory
for a certain resource, meaning it turns out to be a specialized 
final class.
*/
class CHAOS_API ResourceLoader{
	DECLARE_NOTCOPYABLE(ResourceLoader);
	DECLARE_NOTASSIGNABLE(ResourceLoader);

public:
	typedef SharedPtr<ResourceLoader>	Ptr;

	ResourceLoader(){};
	~ResourceLoader(){};

	/*
	A four bytes id for a certain resource
	*/
	virtual int		type() = 0;

	/*
	to load the data and create the corresponding object.

	A resource is denoted by a unique string and will be one copy through the
	application. It has several inner state to indicate if it is loader or not,
	and will reload if needed. It cannot load or even reload itself because it
	doesn't know where itself comes from. The loading procedure will be contr-
	olled by the resource manager. Thus the resource manager maintain a table
	which stores the information corresponds to a named resource.
	*/
	virtual Resource*	load( DataStream* ds, char const* name = 0) = 0;

	/*
	to unload the resource

	NB: it could do nothing.e.g, a script reference should not be unloaded
	because it cannot reload again.
	*/
	virtual void		unload( Resource* res );
};

_CHAOS_END

#endif

