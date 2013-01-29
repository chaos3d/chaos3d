/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_PSCANVAS_H
#define	_CHAOS_PSCANVAS_H

#include "chaos_config.h"
#include "Canvas.h"

_CHAOS_BEGIN

DECLARE_CLASS(PSCanvas, Canvas);
class PSCanvas : public Canvas{

public:
	PSCanvas(){};
	PSCanvas(TypeLua const& lua) : Canvas(lua){};

	/**
	* @see Canvas::elementSize
	*/
	virtual size_t elementSize() const;

	/**
	* @see Canvas::getVertexData
	*/
	virtual void getVertexData( VertexData& ) const;
};

_CHAOS_END

#endif

