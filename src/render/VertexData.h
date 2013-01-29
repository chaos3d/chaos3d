/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_VERTEXDATA_H
#define	_CHAOS_VERTEXDATA_H

#include "chaos_config.h"
#include "VertexAttribute.h"

_CHAOS_BEGIN

class HardwareBuffer;

struct VertexData{
public:
	enum VertexType{
		/// A list of points, 1 vertex per point
        POINT_LIST = 0,
		/// A list of lines, 2 vertices per line
        LINE_LIST = 1,
		/// A loop of list of lines
		LINE_LOOP = 2,
		/// A strip of connected lines, 1 vertex per line plus 1 start vertex
        LINE_STRIP = 3,
		/// A list of triangles, 3 vertices per triangle
        TRIANGLE_LIST = 4,
		/// A strip of triangles, 3 vertices for the first triangle, and 1 per triangle after that 
        TRIANGLE_STRIP = 5,
		/// A fan of triangles, 3 vertices for the first triangle, and 1 per triangle after that
        TRIANGLE_FAN = 6,
	};

	enum IndexType{
		UBYTE,
		USHORT,
	};

	HardwareBuffer*		vertexBuffer;
	HardwareBuffer*		indexBuffer;

	VertexAttribute		vertexAttr[8];
	int					start, count;		// the start offset of data buffer and count
	VertexType			type;
	IndexType			idxType;

	VertexData()
		: vertexBuffer(0), indexBuffer(0), start(0), count(0), type(POINT_LIST), idxType(USHORT)
	{}
};

_CHAOS_END

#endif

