/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "anim/AnimationManager.h"
#include "anim/AnimationState.h"

#include "script/scripting.h"
#include "app/Application.h"

#include <algorithm>

//using namespace chaos;

float AnimationState::time = 0.f;

AnimationManager::AnimationManager(float step)
	: mStep(step), mCurList(0)
{
}

void AnimationManager::play(AnimationState* as){
	if ( !exists( as ) ) {
		append( as );		
	}
}

void AnimationManager::stop(AnimationState* as){
	if( exists( as ) ){
		Sequence::getInstance()->pulse( as );
		remove( as );
	}
}

void AnimationManager::update(){
	Sequence* sequence = Sequence::getInstance();
	AnimationState::time = Timer::getInstance()->getGameTime();
	//Application::getInstance()->global();

	for (iterator it = begin(); it;) {
		AnimationState *cur = &(*it++);
		if ( !cur->run() ) {
			sequence->pulse( cur );
			remove( cur );
		}
	}
}