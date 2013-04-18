/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef _CHAOS_DATASOURCE_H
#define _CHAOS_DATASOURCE_H

#include "common/common.h"
#include "common/ReferencedCount.h"

_CHAOS_BEGIN

class TypeLua;
class Serialization;

/*
to write data into a source
*/
class DataSource : public ReferencedCount{
public:
	enum{
		Seek_Cur,
		Seek_End,
		Seek_Set
	};

	DataSource() {};
	virtual ~DataSource() {};

	/*
	To write the data into the buffer
	*/
	virtual size_t	write(void const* data, size_t size) = 0;
	virtual bool	seek( long offset, int pos = Seek_Cur ) = 0;
	virtual void	close() = 0;
	
	virtual Serialization* asSerialization();
};

_CHAOS_END

#endif
