/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_PARTICLEEMITTEROPERATOR_H
#define	_CHAOS_PARTICLEEMITTEROPERATOR_H

#include "chaos_config.h"
#include "core/RTTI.h"
#include "core/math.h"
#include "PSOperator.h"

_CHAOS_BEGIN

namespace ps{
	/*
	template<class Particle>
	class VelocityEmitter : public PSOperatoer<Particle>{
	protected:
		float	mMin, mRange;

	public:
		VelocityEmitter() :mMin(0.f), mRange(0.f) {};

		inline void	spawn( Particle& p ) const{
			p.dPos = 
		};

		inline void	set( float min_, float max_){
			mMin = min_, mRange = max_-min_;
		}
	};*/

	class PointEmitter : public PSOperator{
	public:
		PointEmitter() {};

		template<class ParticleType>
		inline void spawn(typename ParticleType::Particle& p, typename ParticleType::State& s) const{
			p.pos = s.pos;
			memset(&s.dPos, 0, sizeof(s.dPos));
		};

		template<class ParticleType>
		inline void update (typename ParticleType::Particle& p, typename ParticleType::State const& s){
			p.pos += s.dPos;
		};

		
	};

	/*
	class SphereEmitter : public PSOperator{
	protected:
		float		mRadius;

	public:
		SphereEmitter() : mRadius(0.f) {};

		template<class Particle>
		inline void	spawn( Particle& p, Vector3f const& pos ) const{
			float beta = 1.f/(float)((rand() & 0xFFFF) + 1);
			float gama = 1.f/(float)((rand() & 0xFFFF) + 1);
			//p.pos = pos + mRange * ();
		};

		inline void	set( float radius ){
			mRadius = radius;
		}

		DECLARE_TEMPLATE;
	};

	class LineEmitter : public PSOperator{
	protected:
		Vector3f	mRange;

	public:
		LineEmitter() :mRange(0.f,0.f,0.f) {};

		template<class Particle>
		inline void	spawn( Particle& p, Vector3f const& pos ) const{
			p.pos = pos + mRange * (1.f/(float)((rand() & 0xFFFF) + 1));
		};

		inline void	set( Vector3f const& start, Vector3f const& end ){
			mRange = end-start;
		}
	};

	template<class Particle>
	class RectangleEmitter : public PSOperator{
	protected:
		Vector3f	mRangeX, mRangeY;

	public:
		RectangleEmitter() :mRangeX(Vector3f::ZERO),mRangeY(Vector3f::ZERO) {
		};

		inline void	spawn( Particle& p, Vector3f const& pos ) const{
			//p.pos = pos + mRangeX * (1.f/(float)((rand() & 0xFFFF)+1) + mRangeY * ((rand() & 0xFFFF)+1));
		};

		inline void	set( Rectangle3f const& rect ){
			mRangeX = rect.GetMPCorner();
			mRangeY = rect.GetPMCorner();
		}
	};
	*/

};

_CHAOS_END

#endif

