/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "io/GlobalMDSManager.h"
#include "core/core.h"
#include "core/utils.h"

//using namespace chaos;

GlobalMDSManager::GlobalMDSManager() : mMDSList(ConstCharCompare){
}

//GlobalMDSManager::~GlobalMDSManager(){
//}

bool GlobalMDSManager::createDataStream( const char* name, size_t size ){
	size_t strLen = strlen( name ) + 1;
	size_t realSize = strLen + sizeof(size_t) + size;
	char *address = new char[realSize];

	if( address == 0 ){
		LOG_STAT(ERROR, "Not enough memory for (%s), %lu.", name, (unsigned long)size );
		return false;
	}

	const char *realName = address;
	//void *realAddr = address + strLen + sizeof(size_t);

	strcpy( address, name );	// including terminal null char
	*(size_t*)(address+strLen) = size;
	mMDSList[realName] = address;

	return true;
}

MemoryDataStream* GlobalMDSManager::streamByName( const char* name ){
	char *address = (char*)mMDSList[ name ];

	if( address = 0 ){
		LOG("Memory not created: %s");
		return 0;
	}

	size_t strLen = strlen( address ) + 1;
	size_t size = *(size_t*)address;
	MemoryDataStream* mds = new MemoryDataStream( address + strLen + sizeof(size_t), size );
	mds->autorelease();
	return mds;
}
