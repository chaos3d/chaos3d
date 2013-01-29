/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_DRAWINGUTIL_H
#define	_CHAOS_DRAWINGUTIL_H

#include "chaos_config.h"
#include "2d/Texture.h"
#include "script/LuaType.h"

_CHAOS_BEGIN

/**
 * utility for drawing text
 */
DECLARE_CLASS(DrawingUtil, NilParent);

class DrawingUtil : public Singleton<DrawingUtil>{
public:
	static void drawText(Texture*, char const*, TypeLua const& settings);
};

_CHAOS_END

#endif