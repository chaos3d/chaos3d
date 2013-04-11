/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_PARTICLEGENERICOPERATOR_H
#define	_CHAOS_PARTICLEGENERICOPERATOR_H

#include "chaos_config.h"
#include "core/RTTI.h"
#include "core/math.h"
#include "PSOperator.h"

_CHAOS_BEGIN

namespace ps{

	class LifeOperator : public PSOperator{
	protected:
		float	mMaxInv, mMinInv;

	public:
		LifeOperator() :mMaxInv(1.f), mMinInv(1.f) {};

		bool parse(char const* key, lua_State* L);

		void set( float min_, float max_ );

		template<class ParticleType>
		inline void update (typename ParticleType::Particle& p,typename ParticleType::State const& s){
			p.life -= s.dLife;
		};

		template<class ParticleType>
		inline void spawn(typename ParticleType::Particle& p, typename ParticleType::State& s) const{
			p.life = 1.f;
			s.dLife = mMaxInv + (mMinInv-mMaxInv) * ((float)(rand() & 0xFFFF) / 0xFFFF);
		};
	};

	class ColorOperator : public PSOperator{
	protected:
		ColorRGBA	mStart, mStartRange;
		ColorRGBA	mEnd, mEndRange;

	public:
		ColorOperator()
			: mStart(ColorRGBA::WHITE), mStartRange(0.f,0.f,0.f,0.f),
			mEnd(0.f,0.f,0.f,0.f), mEndRange(0.f,0.f,0.f,0.f)

		{};

		bool parse(char const* key, lua_State* L);

		inline void	set( ColorRGBA const& startLower, ColorRGBA const& startUpper, 
			ColorRGBA const& endLower, ColorRGBA const& endUpper)
		{
			mStart = startLower, mStartRange = (startUpper - startLower);
			mEnd = endLower, mEndRange = (endUpper - endLower);
		}

		template<class ParticleType>
		inline void spawn(typename ParticleType::Particle& p, typename ParticleType::State& s) const{
			p.color = mStart + mStartRange * ((float)(rand() & 0xFFFF) / 0xFFFF);
			s.dColor = (mEnd + mEndRange* ((float)(rand() & 0xFFFF) / 0xFFFF) - p.color) * s.dLife;		// because life is normalized
		};

		template<class ParticleType>
		inline void update (typename ParticleType::Particle& p,typename ParticleType::State const& s){
			p.color += s.dColor;
		};

	};

	class SizeOperator : public PSOperator{
	protected:
		float	mStart, mStartRange;
		float	mEnd, mEndRange;

	public:
		SizeOperator()
			: mStart(1.f), mStartRange(0.f),
			mEnd(0.f), mEndRange(0.f)
		{};

		bool parse(char const* key, lua_State* L);

		inline void	set( float startLower, float startUpper, 
			float endLower, float endUpper)
		{
			mStart = startLower, mStartRange = (startUpper - startLower);
			mEnd = endLower, mEndRange = (endUpper - endLower);
		}

		template<class ParticleType>
		inline void spawn(typename ParticleType::Particle& p, typename ParticleType::State& s) const{
			p.size = mStart + mStartRange * ((float)(rand() & 0xFFFF) / 0xFFFF);
			s.dSize = (mEnd + mEndRange* ((float)(rand() & 0xFFFF) / 0xFFFF) - p.size) * s.dLife;		// because life is normalized
			//p.size = 20.f;
		};

		template<class ParticleType>
		inline void update (typename ParticleType::Particle& p,typename ParticleType::State const& s){
			p.size += s.dSize;
		};

	};

	/*
	class ForceOperator : public PSOperator{
		DECLARE_CLASS(ForceOperator);

	protected:
		Vector3f	mForce;

	public:
		ForceOperator():mForce(0.f,0.f,0.f){};

		inline void	set( Vector3f const& force) {
			mForce = force;
		}

		inline void	set( float x, float y, float z) {
			mForce = Vector3f(x,y,z);
		}

		bool parse(char const* key, lua_State* L);

		template<class ParticleType>
		inline void update (typename ParticleType::Particle& p,typename ParticleType::State& s){
			s.dPos += s.dLife*mForce;
		};

	};*/

	class VectorForceOperator : public PSOperator{
	protected:
		Vector3f	mPos;
		float		mForce;

	public:
		VectorForceOperator():mForce(0.f), mPos(0.f,0.f,0.f){};

		inline void	set( Vector3f const& pos, float force) {
			mForce = force, mPos = pos;
		}

		template<class ParticleType>
		inline void update (typename ParticleType::Particle& p,typename ParticleType::State const& s){
			p.pos += (p.pos - mPos) * mForce * p.dLife;
		};

	};

	struct Particle;
/*		DECLARE_SPEC( LifeOperator, Particle );
	DECLARE_SPEC( ColorOperator, Particle );
	DECLARE_SPEC( SizeOperator, Particle );
	DECLARE_SPEC( ForceOperator, Particle );
	DECLARE_SPEC( VectorForceOperator, Particle );*/
};

DECLARE_CLASS(ps::LifeOperator,NilParent);
DECLARE_CLASS(ps::ColorOperator,NilParent);
DECLARE_CLASS(ps::SizeOperator,NilParent);
DECLARE_CLASS(ps::VectorForceOperator,NilParent);

_CHAOS_END

#endif

