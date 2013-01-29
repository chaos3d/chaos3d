/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_PSSPRITE_H
#define	_CHAOS_PSSPRITE_H

#include "chaos_config.h"
#include "core/math.h"
#include "Sprite.h"

_CHAOS_BEGIN

struct ParticleType{
	struct Particle{
		ColorRGBA	color;
		Vector2f	pos;
		float		size;
		float		life;
	};
	struct State{
		Scene2DNode*	node;
		Vector2f	local;		// local position
		Vector2f	dPos;		// dpos == velocity
		Vector2f	dVel;		// accelarator
		ColorRGBA	dColor;
		float		dSize;
		float		dLife;
		float		radical, tangent;
	};
};

DECLARE_CLASS(PSSprite, Sprite);
class PSSprite : public Sprite{
	DYNAMIC_CLASS(PSSprite);

public:
	typedef ParticleType::Particle Particle;
	typedef ParticleType::State State;

	enum{
		DefaultCapacity = 128
	};

protected:
	Particle	*mParticle;
	State		*mState;
	short		mCount;
	short		mCapacity;
	float		mRate, mCurrent;

	virtual void parseElement(lua_State* L);

public:
	PSSprite(Scene2DNode *node);

	void	setCapacity(int);
	int		getCapacity() const {return mCapacity;};

	void	setRate(float r);
	float	getRate() const { return mRate; };

	Particle*	getParticle() const { return mParticle;};
	State*		getState() const { return mState; };

	int		emit(int count, int c);

	/**
	* @see Sprite::updateData
	*/
	virtual void updateData(short, SpriteData&) const;
};

_CHAOS_END

#endif

