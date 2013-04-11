/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_ANIMATIONSTATE_H
#define	_CHAOS_ANIMATIONSTATE_H

#include "chaos_config.h"
#include "core/core.h"
#include "core/RTTI.h"
#include "script/scripting.h"

_CHAOS_BEGIN

class AnimationManager;

/*
animation state, a specific running animation
a instance data for animations

it is created by a specific animation, like transform animation or
skeleton animation which are shared data among similar animations.
then AnimationState is a controller that outputs/update animated
value from inputs.

Note normalized timeline, ranging from 0.0 to 1.0
*/
DECLARE_CLASS(AnimationState, Triggerable);

class CHAOS_API AnimationState : public Triggerable{
	friend class AnimationManager;
	//DYNAMIC_CLASS(AnimationState);

	DECLARE_NOTCOPYABLE( AnimationState );
	DECLARE_NOTASSIGNABLE( AnimationState );
public:
	typedef	AnimationState	Element;

private:
	static	float	time;		// global time
	
	Element*		mNext;
	Element*		mPrev;

protected:
	float			mStart;		// global start time
								// current time not needed, because of share with global time
	float			mOffset;	// start offset
	float			mRate;		// play rate/speed
	float			mNumPlay;	// number of plays, <=0 == infinite loop
								// fraction part represents normalized timeline

public:
	AnimationState()
		: mStart(time), mRate(1.f), mNumPlay(1), mOffset(0.f), mNext(0), mPrev(0)
	{};

	Element* next() const{
		return mNext;
	};
	
	Element* pre() const{
		return mPrev;
	};
	
	Element*& next() {
		return mNext;
	};
	
	Element*& pre() {
		return mPrev;
	};
	
	static inline float getGlobal() { return time; };

	inline float getStart() const { return mStart; };
	inline float& getStart() { return mStart; };

	// start until s seconds from "current" time
	inline void setStart(float s){ mStart = time + s; };

	inline float getRate() const { return mRate; };
	inline float& getRate() { return mRate; };
	inline void setRate(float r){ mRate = r; };

	inline float getDuration() const { return 1.f/mRate; };
	inline void setDuration(float dur){	mRate = 1.f/dur; };

	inline float getNumPlay() const { return mNumPlay; };
	inline float& getNumPlay() { return mNumPlay; };
	inline void setNumPlay(float n)  { mNumPlay = n; };

	float numOfPlayed() const;
	bool done() const;

	// play animation if activated
	virtual void activated() {
		play( mStart, getDuration(), getNumPlay() );
	};

	// play the animation
	// count <= 0: infinite loop
	virtual void	play(float start, float duration = 1.f, float count = 0.f, float offset = 0.f);

	// stop after count of plays
	// 1, after current play
	// <=0, stop immediately
	virtual void	stop(float count = 0.f);

	// local time
	/*
	inline
	int			local(float& t){
		if( t < mStart )		// not started yet
			return mState = (t = mOffset, NotStarted);
		else if( mState == Done )
			return Done;

		float length = (t - mStart) * mRate;
		float num = (float)(int) (t=length+mOffset);
		if( mNumPlay > 0 && abs(length) >= mNumPlay){		// finite loop and exceed num of plays
			t = fmod( (mRate>0.f?mNumPlay:1.f+num-mNumPlay) + mOffset,1.f);
			if( t <= FLT_EPSILON )
				return mState = ((t=mRate>0.f?1.f:0.f), Done);
			else
				return mState = Done;
		}else
			return mState = ((t = t>=0.f? t-num : t-num+1.f), Normal);			// t modular 1.f
	}

	// local time
	inline
	int			local(float& t, float &length){
		if( t < mStart )		// not started yet
			return mState=((t = mRate>0.f?1.f:0.f), NotStarted);
		else if( mState == Done )
			return Done;

		length = (t - mStart) * fabs(mRate);
		mStart += (float)((int)length)/fabs(mRate);
		if( mNumPlay > 0.f && (mNumPlay -= length) <= 0.f ){		// finite loop and exceed num of plays
			return mState=((t=mRate>0.f?1.f:0.f), Done);
		}else{
			return mState=(t=t>=0.f? t-length : t-length+1.f, Normal);			// t modular 1.f
		}
	}*/

	/**
	* interpolate value and apply to object in global space
	* return false if done
	*/
	bool run(){
		float t = getGlobal();
		if( t < mStart ) return true;

		float length = fabsf((t - mStart) * (mRate));
		int num = (int) length;
		mStart += (float)(num) / fabsf(mRate);
		if( mNumPlay > 0.f ){
			if( mNumPlay > length){
				mNumPlay -= num;
				return update(local(t), num) && mNumPlay > 0.f;
			}else{
				num = (int) mNumPlay;
				mNumPlay = 0.f;
				return update((mRate>0.f?1.f:0.f), num), false;
			}
		}else if( mNumPlay <= -1.f ){	// infinite loop
			return update(local(t), num);
		}else	// keep running last update?
			return update((mRate>0.f?1.f:0.f), 0), false;
	}

	// local time
	/**
	* transform global time to local time
	* assume global time fall in [start, duration*count]
	// todo, move offset to update
	*/
	inline float local(float t){
		float length = (t - mStart) * (mRate);
		return length - (length > 0.f ? (int)length : (int)(length+1.f));
	}

	/**
	* interpolate value and apply to object, 
	* t is in local space [0.f, 1.f]
	* n is number of running times
	* return true if keep running after last update until false
	*    return value only matters after time out
	*/
	virtual bool update(float t, int n = 0) = 0;

	virtual ~AnimationState() {
	};

};

TYPE_RET_REF( AnimationState );
//TYPE_RET_DYN( AnimationState );

_CHAOS_END

#endif

