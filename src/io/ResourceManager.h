/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef _CHAOS_RESOURCEMANAGER_H
#define _CHAOS_RESOURCEMANAGER_H

#include "chaos_config.h"
#include "core/core.h"
#include "core/utils.h"
#include "core/RTTI.h"
#include <map>
#include <list>

#include "Resource.h"
#include "ResourceLoader.h"
#include "ResourceDescriptor.h"

_CHAOS_BEGIN

/** ResourceManager
*/
/*
Alloc policy:
class Alloc{
template<class T>
Type{
	allocate, deallocate, construct, destruct,
	}
}
*/
DECLARE_CLASS(ResourceManager, NilParent);

class CHAOS_API ResourceManager : public Singleton<ResourceManager>{
	DECLARE_NOTCOPYABLE(ResourceManager);
	DECLARE_NOTASSIGNABLE(ResourceManager);

protected:
	
	typedef hash_map<char const*, ResourceDescriptor, HashCompare>	TResTable;
	typedef std::map<int, ResourceLoader::Ptr>		TLoaderTable;

	TResTable		mResource;
	TLoaderTable	mLoaders;

public:
	ResourceManager();
	~ResourceManager();

	/*
	add a list of loaders.
	*/
	void		addLoaders( ResourceLoader::Ptr* loaders );

	/*
	load a resource using the default loaders. the loaded resource will not
	be added into the resource table. so this method is just for loading a
	single, independent resource.
	*/
	Resource*	loadByType( int type, char const* url );
	void		load( ResourceDescriptor& rd );
	//Resource*	loadByType( char const* type, char const* url );

	/*
	add a resource to the manager which can be retreived afterwards
	*/
	void		addResource( int type, char const* url, char const* name);

	/*
	load the specific resource

	NB: resource chunk and resource package are both resources, which
	could be found in the table. it also means that loading one resource
	could lead to another resource being loaded.
	*/
	Resource*	loadByName( char const* name );

	/*
	get a resource
	*/
	Resource*	find( char const* name);
	inline
	Resource*	operator[](char const* name){
		return find( name );
	}

};

_CHAOS_END

#endif
