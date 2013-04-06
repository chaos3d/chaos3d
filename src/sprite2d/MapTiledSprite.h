/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_MAPTILEDSPRITE_H
#define	_CHAOS_MAPTILEDSPRITE_H

#include "chaos_config.h"
#include "Sprite.h"
#include "script/LuaType.h"
#include <vector>

_CHAOS_BEGIN

/*
* Definition ext:
*	type = "map"
*	tile = { {rect}, ..}
*	map = {0~n, ..}
*	width = width of row
*	step = stepX or {stepX, stepY}
*
* i.e.
*	sprite = {canvas, {0,0,25,25}, type="map",
*		tile={{0,0,25,25},{0,0,25,25}}, map={0,0,1,0,1},
*		width = 3, step=1.2}
*/

DECLARE_CLASS(MapTiledSprite, Sprite);
class MapTiledSprite : public Sprite{
	DYNAMIC_CLASS(MapTiledSprite);
public:
	typedef std::vector<int>		TMap;
	typedef std::vector<Rectanglef>	TMapElement;

protected:
	TMapElement	mElement;
	TMap	mMap;
	int		mWidth;
	float	mStepX, mStepY, mStepXY, mStepYX;
	int		mTriangle;

	virtual void parseElement(lua_State* L);

public:
	MapTiledSprite(Scene2DNode *);

	virtual void updateData(short, SpriteData&) const;
	
	void setMap(TypeLua const&);
	TMap&	getMap() { return mMap; };
	TMapElement&	getElement() { return mElement;}

	int		getWidth() const { return mWidth; };
	float	getStepX() const { return mStepX; };
	float	getStepY() const { return mStepY; };
	void	setWidth( int w ) { mWidth = w; };
	void	setStepX( float x ) { mStepX = x; };
	void	setStepY( float y ) { mStepY = y; };
	int		getTriangle() const { return mTriangle;}
	void	setTriangle(int t) { mTriangle = t;}

};

_CHAOS_END

#endif

