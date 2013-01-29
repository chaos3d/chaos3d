/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_VERTEXATTRIBUTE_H
#define	_CHAOS_VERTEXATTRIBUTE_H

#include "chaos_config.h"

_CHAOS_BEGIN

struct VertexAttribute{
	enum Index{
		NONE,
		POSITION,
		COLOR,
		NORMAL,
		TEXCOORD_0,
		TEXCOORD_1,
		TEXCOORD_MAX = TEXCOORD_1,
		POINT_SPRITE_SIZE,
		USERDEFINED,
	};

	enum Type{
		FLOAT,
		CHAR,
	};

	int		offset;
	int		stride;
	int		size;
	Index	idx;
	Type	type;

	VertexAttribute():idx(NONE){};

	VertexAttribute(int _offset,int _stride, int _size, Index _idx, Type _type) 
		: offset(_offset), stride(_stride), size(_size), idx(_idx), type(_type)
	{};
};

_CHAOS_END

#endif
