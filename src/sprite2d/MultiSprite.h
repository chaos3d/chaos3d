/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_MULTISPRITE_H
#define	_CHAOS_MULTISPRITE_H

#include "chaos_config.h"
#include "Sprite.h"
#include <vector>

_CHAOS_BEGIN

/*
* Definition ext:
*	type = "multi"
*	sub = { {rect, [frame]}, ..}
* i.e.
*	sprite = {canvas, {any}, type="multi",
*		sub={{0,0,25,25},{0,0,25,25,-10,-10,20,20}}
*		}
*/

DECLARE_CLASS(MultiSprite, Sprite);
class MultiSprite : public Sprite{
	DYNAMIC_CLASS(MultiSprite);

public:
	struct SubSprite{
		Rectanglef	frame;
		Rectanglef	bound;
		SubSprite();
		SubSprite(Rectanglef const&b, Rectanglef const& f)
			: frame(f), bound(b)
		{};
		SubSprite(Rectanglef const&b)
			: bound(b), 
			frame(Vector2f(-b.Extent[0]/2.f,-b.Extent[1]/2.f), b.Extent[0],b.Extent[1])
		{};
	};

	typedef std::vector<SubSprite>	TSubSprite;

protected:
	TSubSprite	mSubSprite;

	virtual void parseElement(lua_State* L);

public:
	MultiSprite(Scene2DNode *node) : Sprite(node) {};

	TSubSprite& getSubSprite() {return mSubSprite;};
	void setSubFrame(int, float,float,float,float);
	void setSubBound(int, float,float,float,float);

	virtual void updateData(short, SpriteData&) const;
};

_CHAOS_END

#endif
