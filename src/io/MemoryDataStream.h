/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_MEMORYDATASTREAM_H
#define	_CHAOS_MEMORYDATASTREAM_H

#include "DataStream.h"

_CHAOS_BEGIN

/*
Make a piece of memory be a stream, but not manage its lifetime.
*/
class CHAOS_PRIVATE MemoryDataStream : public DataStream{
private:
	char*	mAddress;
	char*	mEnd;
	char*	mCurrent;

public:
	MemoryDataStream( char* address, size_t size);

	virtual bool	valid();
	virtual bool	end();
	virtual size_t	read( void* buf, size_t bufSize );
	virtual bool	seek( long offset, int pos );
	virtual long	tell();
	virtual char const* where();
	virtual void	close() {};
};

_CHAOS_END

#endif
