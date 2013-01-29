/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_TILEDSPRITE_H
#define	_CHAOS_TILEDSPRITE_H

#include "chaos_config.h"
#include "Sprite.h"

_CHAOS_BEGIN

DECLARE_CLASS(TiledSprite, Sprite);
class TiledSprite : public Sprite{

protected:
	int		mWidth, mHeight;

	virtual void parseElement(lua_State* L);
public:
	TiledSprite(Scene2DNode *);

	virtual void updateData(short, SpriteData&) const;

	int		getWidth() const { return mWidth; };
	int		getHeight() const { return mHeight; };
	void	setWidth( int w ) { mWidth = w; };
	void	setHeight( int h ) { mHeight = h; };
};

_CHAOS_END

#endif

