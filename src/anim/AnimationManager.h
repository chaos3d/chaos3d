/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_ANIMATIONMANAGER_H
#define	_CHAOS_ANIMATIONMANAGER_H

#include "chaos_config.h"
#include "core/core.h"
#include <list>

_CHAOS_BEGIN

class AnimationState;

/*
*/
class CHAOS_API AnimationManager : public Singleton<AnimationManager>, public ElementList<AnimationState>{
public:
	typedef std::list<AnimationState*>	TAnimationList;

protected:
	float			mStep;		// time per each frame, 1/fps => 1/30
	TAnimationList	mAnimList[2];
	int				mCurList;

public:
	AnimationManager(float step = 1.f/30.f);

	void	play(AnimationState* as);
	void	stop(AnimationState* as);

	// steps
	float	getStep() const { return mStep; };
	void	setStep(float s){ mStep = s; };

	void	update();
};

_CHAOS_END

#endif

