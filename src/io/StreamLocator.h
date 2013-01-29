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

#include "chaos_config.h"
#include <string>
#include <map>

_CHAOS_BEGIN

class DataStream;

class CHAOS_API StreamLocator{
	DECLARE_NOTCOPYABLE(StreamLocator);
	DECLARE_NOTASSIGNABLE(StreamLocator);
public:
	StreamLocator(){};
	virtual ~StreamLocator(){};

	// check if it exists in the locator
	virtual bool	exist( const char* path ) = 0;

	// get the stream
	virtual DataStream*	stream( const char* url) = 0;		

	// get the universal name
	virtual std::string	url( const char* path) = 0;
};

namespace io{

	class CHAOS_PRIVATE PkgFileLocator : public StreamLocator{
	protected:
		typedef std::multimap<size_t, long>	TFileHash;

		TFileHash		mHash;
		std::string		mFilename;
		DataStream*		mPkgFile;

	public:
		PkgFileLocator( const char* pkg );
		~PkgFileLocator();

		virtual bool	exist( const char* path );
		virtual DataStream*	stream( const char* path);
		virtual std::string	url( const char* path);
	};

	class CHAOS_PRIVATE DirLocator : public StreamLocator{
	protected:
		std::string		mBase;

	public:
		DirLocator( const char* base ) : mBase(base){};

		virtual bool	exist( const char* path );
		virtual DataStream*	stream( const char* path);
		virtual std::string	url( const char* path);
	};

};

_CHAOS_END

#endif

