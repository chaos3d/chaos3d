/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_GLOBALMDSMANAGER_H
#define	_CHAOS_GLOBALMDSMANAGER_H

#include "core/core.h"
#include "MemoryDataStream.h"

#include <map>

_CHAOS_BEGIN

/*
Global MemoryDataStream Manager, 
to manage the named memory data stream that are persistent through the application.
*/

class CHAOS_PRIVATE GlobalMDSManager : public Singleton<GlobalMDSManager>{
protected:
	typedef	std::map<const char*, void*, bool(*)(const char*,const char*)>	TMDSList;

	TMDSList		mMDSList;
public:
	GlobalMDSManager();

	/*
	create a size of memory that could be retreived by the name after.
	*/
	bool	createDataStream( const char* name, size_t size );

	/*
	retreive a memory by the name
	*/
	MemoryDataStream*	streamByName( const char* name );
};

_CHAOS_END

#endif
