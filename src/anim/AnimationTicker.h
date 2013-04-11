/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_ANIMATIONTICKER_H
#define	_CHAOS_ANIMATIONTICKER_H

#include "chaos_config.h"
#include "AnimationState.h"

_CHAOS_BEGIN

class AnimationTicker : public AnimationState{
public:
	virtual bool update(float t, int n){
		return false;
	}
};

_CHAOS_END

#endif
