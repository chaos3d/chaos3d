/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_CHUNKLOADER_H
#define	_CHAOS_CHUNKLOADER_H

#include "ResourceLoader.h"
#include "Resource.h"

_CHAOS_BEGIN

/*
load a chunk
*/
class CHAOS_PRIVATE ChunkLoader : public ResourceLoader{
public:
	virtual int		type(){
		return 'chk ';
	};

	virtual Resource*	load( DataStream* ds, char const* name );
	virtual void		unload( Resource* res );

};

/*
this is a helper class that represents a resource chunk that could be
refered and then unloaded.
*/
class ResourceChunk : public Resource{
protected:
	// just save the resource location
	// when loading, read data from url and then release these data
	// when unloading, read data again.
	const char*		mName;

public:
	ResourceChunk(const char* name) : mName( name ){
	};
};

_CHAOS_END

#endif
