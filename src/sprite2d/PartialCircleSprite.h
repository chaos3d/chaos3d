/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_PARTIALCIRCLESPRITE_H
#define	_CHAOS_PARTIALCIRCLESPRITE_H

#include "chaos_config.h"
#include "PartialSprite.h"

_CHAOS_BEGIN

class PartialCircleSprite : public PartialSprite{
public:
	PartialCircleSprite( Scene2DNode *);

	virtual void setPercent(float percent);
	virtual void updateData(short startIndex, SpriteData& sd) const;
};

_CHAOS_END

#endif

