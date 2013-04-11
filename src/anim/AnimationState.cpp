/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "anim/AnimationState.h"
#include "anim/AnimationManager.h"
#include "script/bind.h"
#include "app/Application.h"
#include <cfloat>

//using namespace chaos;
IMPLEMENT_CLASS(AnimationState, LIB_DOMAIN)
IMPLEMENT_FUNC( play, &AnimationState::play)
IMPLEMENT_FUNC( stop, &AnimationState::stop)
IMPLEMENT_FUNC( update, &AnimationState::update)
IMPLEMENT_PROP( duration, &AnimationState::setDuration, &AnimationState::getDuration)
IMPLEMENT_PROP( start, &AnimationState::setStart, (float (AnimationState::*)() const)&AnimationState::setStart)
IMPLEMENT_PROP( repeat, &AnimationState::setNumPlay, (float (AnimationState::*)() const)&AnimationState::getNumPlay)
IMPLEMENT_END;

float AnimationState::numOfPlayed() const{
	if( time < mStart )		// not started yet
		return 0;

	float t = (time - mStart) * mRate;
	float num = fabs(t);

	if( mNumPlay > 0 && num >= mNumPlay )
		return mNumPlay;
	else
		return num; 
}

bool AnimationState::done() const{
	if( time < mStart )
		return false;

	if( mNumPlay <= FLT_EPSILON )	// infinite loop never stops
		return true;

	float length = (time - mStart) * mRate;
	return abs(length) >= mNumPlay;
}

void AnimationState::play(float start, float duration, float count, float offset){
	mStart = time + start;
	mOffset = offset;
	if( fabs(duration - FLT_EPSILON) > FLT_EPSILON )
		mRate = 1.f / duration;
	else
		mRate = 1.f / 30.f;//TODO Application::getInstance()->getDesiredFps();	// at lease 1 fps 

	if( count < FLT_EPSILON)
		mNumPlay = -1.f;
	else
		mNumPlay = count;

	this->retain();
	AnimationManager::getInstance()->stop(this);
	AnimationManager::getInstance()->play(this);
	this->release();
}

void AnimationState::stop(float count){
	if( count <= 0 )
		AnimationManager::getInstance()->stop(this);
	else
		mNumPlay = (int)numOfPlayed() + count;
}
