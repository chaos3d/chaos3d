/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_FILESTREAM_H
#define	_CHAOS_FILESTREAM_H

#include "chaos_config.h"
#include "DataStream.h"
#include <cstdio>
#include <string>

_CHAOS_BEGIN

class CHAOS_PRIVATE FileStream : public DataStream{
protected:
	FILE	*mFile;
	std::string const	mName;

public:
	~FileStream();
	FileStream( char const* filename );

	static	bool	exist( const char* filename );

	virtual bool	valid();
	virtual void	close();
	virtual bool	end();
	virtual size_t	read( void* buf, size_t bufSize );
	virtual bool	seek( long offset, int pos );
	virtual long	tell();
	virtual char const* where() { return mName.c_str(); }
};

_CHAOS_END

#endif

