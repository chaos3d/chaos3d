/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "res/ResourceManager.h"

#include "res/ResourceLoader.h"
#include "res/ScriptLoader.h"
#include "res/ChunkLoader.h"
#include "res/AudioLoader.h"
#include "io/IOManager.h"
#include "script/bind.h"

//using namespace chaos;
using namespace std;

IMPLEMENT_CLASS( ResourceManager, LIB_DOMAIN )
IMPLEMENT_FUNC( loadByType, &ResourceManager::loadByType)
IMPLEMENT_FUNC( loadByName, &ResourceManager::loadByName)
IMPLEMENT_END;

ResourceManager::~ResourceManager(){
	for( TResTable::iterator it = mResource.begin();
		it != mResource.end();
		++it)
	{
		it->second.resource->release();
	}
}

ResourceManager::ResourceManager(){
	ResourceLoader::Ptr loaders[] = {
		ResourceLoader::Ptr(new ScriptRefLoader()),
		ResourceLoader::Ptr(new ScriptLoader()),
		ResourceLoader::Ptr(new ChunkLoader()),
		ResourceLoader::Ptr(new ScriptResourceLoader()),
		ResourceLoader::Ptr(new AudioLoader()),
		ResourceLoader::Ptr(0),
	};

	addLoaders( loaders );
}

void ResourceManager::addLoaders( ResourceLoader::Ptr* loaders ){
	if( loaders == 0 )
		return ;

	for(; loaders->getPointer() != 0; ++loaders )
		mLoaders[(*loaders)->type()] = *loaders;
}

Resource* ResourceManager::loadByType( int type, char const* url ){
	//Resource::sCurrentAllocator = &mLSRAllocator;
	ResourceLoader::Ptr ptr = mLoaders[ type ];
	Resource *res = 0;
	if( ptr.getPointer() == 0 ){
		LOG("Unable to find a loader for the resource: %s", url);
	}else{
		DataStream* ds = IOManager::getInstance()->streamByURL( url );

		if( ds != 0 ){
			res = ptr->load( ds, 0);
#ifdef	DEBUG
			LOG("Resource at (%s) has been loaded.", url);
#endif
		}else
			LOG("Resource at (%s) has NOT been loaded.", url);
	}
	return res;
}
/*

Resource* ResourceManager::loadByType( char const* type, char const* url ){
#ifdef	WIN32
	//char tmp[4] = {*(type+3), *(type+2), *(type+1), *(type+0)};
	int t = *type;
	*(type+1) == 0 || 
	(
		(t = (t<<8) | *(type+1)),
		( *(type+2) == 0 ||
			( (t = (t<<8) | *(type+2)), 
				( *(type+3) == 0 ||
				(t = (t<<8) | *(type+3)) )
			)
		)
	);
	return loadByType( t, url );
#else
	return loadByType( *(int*)type, url );
#endif
}
*/
Resource* ResourceManager::loadByName( char const* name ){
	TResTable::iterator it = mResource.find( name );
	if( it == mResource.end() ){
#ifdef	DEBUG
		LOG("Unable to find the resource (%s). return nil.", name);
#endif
		return 0;
	}

	ResourceDescriptor& rd = it->second;
	if( rd.state != Loaded ){
		load( rd );
	}
	return rd.resource;
}

void ResourceManager::load( ResourceDescriptor& rd ){
	if( rd.state == Loaded )
		return;

	rd.resource = loadByType( rd.type, rd.url );
	rd.state = Loaded;
}

Resource* ResourceManager::find( char const* name){
	TResTable::iterator it = mResource.find(name);
	if( it == mResource.end() ){
#ifdef	DEBUG
		LOG("Unable to find the resource (%s). return nil.", name);
#endif
		return 0;
	}

	ResourceDescriptor& rd = it->second;
	if( rd.state != Loaded ){
		LOG("Resource (%s) has not yet been loaded. loading now.", name);
		rd.resource = loadByType( rd.type, rd.url );
	}
	return rd.resource;
}

void ResourceManager::addResource( int type, char const* url, char const* name){
	if( mResource.find(name) != mResource.end() ){
		LOG("Resource (%s) has already been defined.", name);
		return;
	}

	size_t url_l = strlen(url), name_l = strlen(name);
	char *desc = new char[ url_l + name_l + 2];
	memcpy( desc, name, name_l+1 );
	memcpy( desc+name_l+1, url, url_l+1 );
	name = desc, url = desc+name_l+1;

	mResource[name] = ResourceDescriptor( type, name, url );
}