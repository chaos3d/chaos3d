/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_FILESOURCE_H
#define	_CHAOS_FILESOURCE_H

#include "chaos_config.h"
#include "DataSource.h"
#include <cstdio>

_CHAOS_BEGIN

class FileSource : public DataSource{
protected:
	FILE*	mFile;

public:
	FileSource( char const* name, char const* mode );
	FileSource( char const* name );
	~FileSource();

	virtual size_t	write(void const* data, size_t size);
	virtual bool	seek( long offset, int pos = Seek_Cur );
	virtual void	close();
};

_CHAOS_END

#endif

