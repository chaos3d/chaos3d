/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_CONCRETEANIMATION_H
#define	_CHAOS_CONCRETEANIMATION_H

#include "chaos_config.h"
#include "core/math.h"
#include "core/RTTI.h"
#include "script/LuaType.h"
#include "res/Resource.h"

#include <vector>
#include <algorithm>
#include <cstdarg>
#include "AnimationState.h"

_CHAOS_BEGIN

template<class Value> class ConcreteAnimation;

template<class Applier, class Value, class TE = LinearTiming>
class CHAOS_API ConcreteAnimationState : public AnimationState{
	typedef ConcreteAnimation<Value>		AnimationType;
protected:
	AnimationType*	mAnimation;
	Applier			mApplier;

public:
	ConcreteAnimationState(AnimationType* anim, Applier const& applier = Applier() )
		: mAnimation(anim), mApplier(applier)
	{
		ASSERT(anim != 0 );
		anim->retain();
	};

	~ConcreteAnimationState(){
		mAnimation->release();
	}

	virtual bool update(float t, int c);
};

template<class Value>
class CHAOS_API ConcreteAnimation : public Resource{
public:
	struct KeyFrame{
		Value	value;
		float	stamp;

		KeyFrame( float s = 0.f ) : stamp(s){};

		inline bool operator<(KeyFrame const& rhs) const{
			return stamp < rhs.stamp;
		};
	};

	typedef std::vector<KeyFrame>	TKFList;

protected:
	TKFList		mKeyFrames;
	bool		mLoop;

public:
	ConcreteAnimation() : mLoop(false){};

	inline TKFList&	getKeyFrames() { return mKeyFrames; };

	template<class Applier, class TE>
	ConcreteAnimationState<Applier, Value, TE>* createState(Applier const& applier){
		ConcreteAnimationState<Applier, Value, TE>* as = 
			new ConcreteAnimationState<Applier, Value, TE>(this, applier);
		as->autorelease();
		return as;
	};

	KeyFrame const&	keyFrame( float t ){
		typename TKFList::iterator it = std::lower_bound( mKeyFrames.begin(), mKeyFrames.end(), KeyFrame(t) );

		if( it == mKeyFrames.end() )
			return mLoop ? mKeyFrames.front() : mKeyFrames.back();
		else
			return *it;
	}

};

template<class Applier, class Value, class TE>
bool ConcreteAnimationState<Applier,Value,TE>::update(float t, int c){
	typename ConcreteAnimation<Value>::KeyFrame const& kf = mAnimation->keyFrame( TE()(t) );
	return mApplier( kf.value );
}

/*
	template<class Applier>
	SpriteAnimationState<Applier>::SpriteAnimationState(SpriteAnimation* anim, Applier const& applier )
	: mAnimation(anim), mApplier(applier)
	{	anim->retain();
	};
	
	template<class Applier>
	SpriteAnimationState<Applier>::~SpriteAnimationState(){
		mAnimation->release();
	}
	*/

_CHAOS_END

#endif
