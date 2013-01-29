// ----------------------------------------------
//
// RenderOperation.h
//
// Created by wuh (2/21)
// Copyright (c) 2009
//
// ----------------------------------------------


#ifndef	_CHAOS_RENDEROPERATION_H
#define	_CHAOS_RENDEROPERATION_H

namespace chaos{

	class VertexBuffer;
	class VertexIndexBuffer;
	class Material;
	/*
	A single draw call that can be sorted.
	*/
	struct RenderOperation{
		enum OperationType{
			/// A list of points, 1 vertex per point
            OT_POINT_LIST = 1,
			/// A list of lines, 2 vertices per line
            OT_LINE_LIST = 2,
			/// A strip of connected lines, 1 vertex per line plus 1 start vertex
            OT_LINE_STRIP = 3,
			/// A list of triangles, 3 vertices per triangle
            OT_TRIANGLE_LIST = 4,
			/// A strip of triangles, 3 vertices for the first triangle, and 1 per triangle after that 
            OT_TRIANGLE_STRIP = 5,
			/// A fan of triangles, 3 vertices for the first triangle, and 1 per triangle after that
            OT_TRIANGLE_FAN = 6
		};

		// priority key for rendering order
		long				key;	//supposed to change

		Material*			material;
		VertexBuffer*		vertexBuffer;
		VertexIndexBuffer*	indexBuffer;

		OperationType		type;

		RenderOperation(Material *_m, VertexBuffer *_vb, 
			VertexIndexBuffer *_ib, OperationType _type);
	};
};

#endif

