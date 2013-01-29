/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef _CHAOS_SERIALIZATION_H
#define _CHAOS_SERIALIZATION_H

#include "chaos_config.h"
#include "core/core.h"
#include "core/RTTI.h"
#include "script/LuaType.h"

_CHAOS_BEGIN

class DataSource;
class DataStream;

/*
 to write data into a source
 */
DECLARE_CLASS(Serialization, NilParent);
class CHAOS_API Serialization : public ReferencedCount{
	
	DECLARE_NOTCOPYABLE(Serialization);
	DECLARE_NOTASSIGNABLE(Serialization);
	
protected:
	DataSource*		mSource;
	
public:
	Serialization(DataSource* s) : mSource(s) {};
	virtual ~Serialization() {};
	
	virtual void save( TypeLua const& );
	
	//
	// todo: save native objects
};

TYPE_RET_REF( Serialization );

/*
 to read data from a stream
 */
DECLARE_CLASS(DeSerialization, NilParent);
class CHAOS_API DeSerialization : public ReferencedCount{
	DECLARE_NOTCOPYABLE(DeSerialization);
	DECLARE_NOTASSIGNABLE(DeSerialization);
	
protected:
	DataStream*		mStream;
	
public:
	DeSerialization(DataStream* s) : mStream(s) {};
	virtual ~DeSerialization() {};
	
	virtual TypeLua restore();
	virtual bool end();
	
	// todo: read native objects
};

TYPE_RET_REF( DeSerialization );

_CHAOS_END

#endif
