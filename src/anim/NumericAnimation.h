/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_NUMERICANIMATION_H
#define	_CHAOS_NUMERICANIMATION_H

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

/*
It is a generic implementation for numeric value animations.
By numeric, it means the value can do arithmetic, say times
and substraction.
*/
template<class Numeric>
class NumericAnimation;
class LinearTiming;

template<class Numeric>
class CHAOS_API LinearAnimationState : public AnimationState{
	//DYNAMIC_CLASS;

public:
	typedef NumericAnimation<Numeric>	AnimationType;

protected:
	AnimationType*	mAnimation;

public:
	LinearAnimationState(AnimationType* anim)
		: mAnimation(anim)
	{
		ASSERT(anim != 0 );
		anim->retain();
	};

	~LinearAnimationState(){
		mAnimation->release();
	};

	/**
	* get the keyframes
	* @remark
	*	it may be referenced by other animation states, cautious if modified
	*/
	AnimationType*	animation() const{
		return mAnimation;
	};
};

template<class Applier, class Numeric, class TE = LinearTiming>
class CHAOS_API NumericAnimationState : public LinearAnimationState<Numeric>{
	//DECLARE_NOTCOPYABLE(NumericAnimationState);
	//DECLARE_NOTASSIGNABLE(NumericAnimationState);
public:
	typedef typename LinearAnimationState<Numeric>::AnimationType	AnimationType;
	using LinearAnimationState<Numeric>::mAnimation;
	using AnimationState::getGlobal;
	using AnimationState::local;
	
protected:
	Applier			mApplier;
	
public:
	NumericAnimationState(AnimationType* anim, Applier const& applier = Applier() )
		: LinearAnimationState<Numeric>(anim), mApplier(applier)
	{};

	virtual bool update(float t, int c);
};

template<class Numeric>
class CHAOS_API NumericAnimation : public Resource{
	//DECLARE_NOTCOPYABLE(NumericAnimation);
	//DECLARE_NOTASSIGNABLE(NumericAnimation);
public:
	struct KeyFrame{
		Numeric		value;
		float		stamp;

		KeyFrame( float s = 0.f) : stamp(s) {};
		KeyFrame( Numeric const& val, float s ) 
			: stamp(s), value(val) {};

		inline
		bool operator<(KeyFrame const& rhs) const{
			return stamp < rhs.stamp;
		};
	};


	typedef std::vector<KeyFrame>	TKFList;
	
protected:
	TKFList		mKeyFrames;
	bool		mLoop;

public:
	NumericAnimation(TypeLua const& lua);

	NumericAnimation(bool loop, KeyFrame* kf, ...) : mLoop(loop){
		va_list kfs;
		va_start( kfs, kf );
		//if( kf != 0 )
		//	mKeyFrames.push_back( *kf );

		for( ; kf != 0; kf = va_arg( kfs, KeyFrame *)){
			mKeyFrames.push_back( *kf );
		}

		std::sort( mKeyFrames.begin(), mKeyFrames.end() );
		va_end( kfs );
 	};

	NumericAnimation(size_t n = 2, bool loop = false) : mLoop(loop), mKeyFrames(0)
	{
		mKeyFrames.reserve(n);
	};

	// helper function for script
	template<class T, class TE>
	AnimationState*	animation( typename T::Appliee* t ){
		if( t == 0 )
			return 0;

		typedef NumericAnimationState<typename T::Applier, Numeric, TE> NAS;
		NAS* as = new NAS(this, typename T::Applier(t));
		as->autorelease();
		return as;
	}

	inline TKFList&	getKeyFrames() { return mKeyFrames; };

	template<class Applier, class TE>
	NumericAnimationState<Applier, Numeric, TE>*
		createState(Applier const& applier){
			NumericAnimationState<Applier, Numeric, TE>* as = 
				new NumericAnimationState<Applier, Numeric, TE>(this, applier);
			as->autorelease();
			return as;
	};

	KeyFrame	keyFrame( float t ){
		KeyFrame kf(t);
		typename TKFList::iterator it = std::upper_bound( mKeyFrames.begin(), mKeyFrames.end(), kf );
		typename TKFList::iterator pre = it;
		float last = 1.f;

		if( it == mKeyFrames.begin() )
			return mKeyFrames.front();
		else if( it == mKeyFrames.end() ){
			if( !mLoop )
				return mKeyFrames.back();
			else{
				pre = it - 1;
				it = mKeyFrames.begin();
			}
		}else{
			--pre;
			last = it->stamp;
		}

		float beta = (t-pre->stamp)/(last-pre->stamp);
		(((kf.value = it->value) -= pre->value) *= beta) += pre->value;
		return kf;
	}

};

typedef LinearAnimationState<Vector3f>		Vector3fAnimationState;
typedef LinearAnimationState<Vector2f>		Vector2fAnimationState;
typedef LinearAnimationState<Quaternionf>	QuaternionAnimationState;
typedef LinearAnimationState<ColorRGBA>		ColorAnimationState;
typedef LinearAnimationState<float>			ValueAnimationState;

///// No need?
/*
DECLARE_CLASS_A(Vector3fAnimationState, AnimationState);
DECLARE_CLASS_A(Vector2fAnimationState, AnimationState);
DECLARE_CLASS_A(QuaternionAnimationState, AnimationState);
DECLARE_CLASS_A(ColorAnimationState, AnimationState);
DECLARE_CLASS_A(ValueAnimationState, AnimationState);

TYPE_RET_REF( Vector3fAnimationState );
TYPE_RET_REF( Vector2fAnimationState );
TYPE_RET_REF( QuaternionAnimationState );
TYPE_RET_REF( ColorAnimationState );
TYPE_RET_REF( ValueAnimationState );
*//////

typedef NumericAnimation<Vector3f>		Vector3fAnimation;
typedef NumericAnimation<Vector2f>		Vector2fAnimation;
typedef NumericAnimation<Quaternionf>	QuaternionAnimation;
typedef NumericAnimation<ColorRGBA>		ColorAnimation;
typedef NumericAnimation<float>			ValueAnimation;

DECLARE_CLASS_A(Vector3fAnimation, NilParent);
DECLARE_CLASS_A(Vector2fAnimation, NilParent);
DECLARE_CLASS_A(QuaternionAnimation, NilParent);
DECLARE_CLASS_A(ColorAnimation, NilParent);
DECLARE_CLASS_A(ValueAnimation, NilParent);

TYPE_RET_REF( Vector3fAnimation );
TYPE_RET_REF( Vector2fAnimation );
TYPE_RET_REF( QuaternionAnimation );
TYPE_RET_REF( ColorAnimation );
TYPE_RET_REF( ValueAnimation );

template<> NumericAnimation<Vector3f>::NumericAnimation(TypeLua const& lua);
template<> NumericAnimation<Vector2f>::NumericAnimation(TypeLua const& lua);
template<> NumericAnimation<QuaternionAnimation>::NumericAnimation(TypeLua const& lua);
template<> NumericAnimation<ColorAnimation>::NumericAnimation(TypeLua const& lua);
template<> NumericAnimation<ValueAnimation>::NumericAnimation(TypeLua const& lua);

template<class Applier, class Numeric, class TE>
bool NumericAnimationState<Applier,Numeric,TE>::update(float t, int c){
	typename NumericAnimation<Numeric>::KeyFrame kf
		= mAnimation->keyFrame( TE()(t) );
	return mApplier( kf.value );
}

_CHAOS_END

#endif

