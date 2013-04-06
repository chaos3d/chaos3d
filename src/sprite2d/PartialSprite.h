/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_PARTIALSPRITE_H
#define	_CHAOS_PARTIALSPRITE_H

#include "chaos_config.h"
#include "Sprite.h"

_CHAOS_BEGIN

DECLARE_CLASS(PartialSprite, Sprite);
class PartialSprite : public Sprite{
	DYNAMIC_CLASS(PartialSprite);
protected:
	Rectanglef	mOriginBound, mOriginFrame;
	float		mPercent;
	bool		mVertical;

	virtual void parseElement(lua_State* L);

public:
	PartialSprite( Scene2DNode *);

	void	resetOrigin();

	Rectanglef const&	getOriginBound() const { return mOriginBound; };
	Rectanglef const&	getOriginFrame() const { return mOriginFrame; };
	void	setVertical(bool v) { mVertical = v; };
	bool	getVertical() const { return mVertical; };
	virtual void	setPercent(float percent);
	float	getPercent() const { return mPercent; };

};

_CHAOS_END

#endif

