/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef _CHAOS_DATASTTREAM_H
#define _CHAOS_DATASTTREAM_H

#include "chaos_config.h"
#include "core/core.h"
#include "core/RTTI.h"

_CHAOS_BEGIN

class DeSerialization;

DECLARE_CLASS(DataStream, NilParent);
class CHAOS_API DataStream : public ReferencedCount{

	DECLARE_NOTCOPYABLE(DataStream);
	DECLARE_NOTASSIGNABLE(DataStream);

public:
	enum{
		Seek_Cur,
		Seek_End,
		Seek_Set
	};

	DataStream(){};
	virtual ~DataStream() {};

	virtual bool	valid() = 0;
	virtual bool	end() = 0;
	virtual size_t	read( void* buf, size_t bufSize ) = 0;
	virtual bool	seek( long offset, int pos = Seek_Cur ) = 0;
	virtual long	tell() = 0;
	virtual void	close() = 0;
	virtual char const* where() = 0;

	virtual DeSerialization* asDeSerialization();

};

TYPE_RET_REF( DataStream );

_CHAOS_END

#endif
