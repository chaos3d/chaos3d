/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_PKGCHUNKSTREAM_H
#define	_CHAOS_PKGCHUNKSTREAM_H

#include "DataStream.h"
#include <string>

_CHAOS_BEGIN

class CHAOS_PRIVATE PkgChunkStream : public DataStream{
protected:
	DataStream*	mStream;
	size_t	mSize;
	long	mOffset, mStartOff;
	std::string const mName;

public:
	~PkgChunkStream();
	PkgChunkStream(DataStream* ds, long offset, size_t size, char const* name);

	virtual bool	valid();
	virtual bool	end();
	virtual size_t	read(void* buf, size_t bufSize);
	virtual bool	seek(long offset, int pos);
	virtual long	tell();
	virtual void	close();
	virtual char const* where() { return mName.c_str(); };
};

_CHAOS_END

#endif

