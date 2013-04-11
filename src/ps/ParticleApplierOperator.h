/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_PARTICLEAPPLIEROPERATOR_H
#define	_CHAOS_PARTICLEAPPLIEROPERATOR_H

#include "ParticleSystem.h"
#include "scene/2d/PointSpriteCollection.h"

_CHAOS_BEGIN

/*
to add the particle into point sprite renderer.
*/
template<class Particle>
class PointSpriteOperator{
protected:
	PointSpriteCollection	*mSprite;

public:
	PointSpriteCollection() : mSprite() {};

	inline
	void	set(PointSpriteCollection *ps){
		mSprite = ps;
	};

	inline
	void	spawn( Particle& p ) const{};
	
	inline
	PointSpriteOperator&	self(){	return *this;};
	
	inline
	void operator() (Particle& p) const {
		mSprite->add ( p.size, p.pos, p.color );
	}

	inline
	void	finalize (){
		mSprite->commit ();
	};
};

_CHAOS_END

#endif

