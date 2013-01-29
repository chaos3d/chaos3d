/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_PARTICLE2DOPERATOR_H
#define	_CHAOS_PARTICLE2DOPERATOR_H

#include "chaos_config.h"
#include "core/RTTI.h"
#include "core/math.h"
#include "ps/PSOperator.h"
#include "PSSprite.h"
#include "Scene2DNode.h"
#include "Transform.h"

_CHAOS_BEGIN

namespace ps{

	class PSSpriteOperator : public PSOperator{
	public:
		PSSpriteOperator() {};
		
		template<class ParticleType>
		inline void spawn(typename ParticleType::Particle& p, typename ParticleType::State& s) const{
			p.pos = Vector2f(s.node->getTransform()->getMatrix()(0,3),
							 s.node->getTransform()->getMatrix()(1,3));
			s.dPos = Vector2f(0.f, 0.f);
			s.dVel = Vector2f(0.f, 0.f);
		};
		
		template<class ParticleType>
		inline void update (typename ParticleType::Particle& p, typename ParticleType::State& s){
			p.pos += s.dPos;
			s.dPos += s.dVel;
		};
	};
	
	class PSTransformOperator : public PSOperator{
	public:
		PSTransformOperator() {};
		
		template<class ParticleType>
		inline void spawn(typename ParticleType::Particle& p, typename ParticleType::State& s) const{
			s.local = Vector2f(s.node->getTransform()->getTranslate()[0],
							 s.node->getTransform()->getTranslate()[1]);
			s.dPos = Vector2f(0.f, 0.f);
			s.dVel = Vector2f(0.f, 0.f);
		};
		
		template<class ParticleType>
		inline void update (typename ParticleType::Particle& p, typename ParticleType::State& s){
			s.local += s.dPos;
			s.dPos += s.dVel;
			if ( s.node->getParent() != 0 && s.node->getParent()->getTransform() != 0 ) {
				Vector4f const& ret = s.node->getParent()->getTransform()->getMatrix() * Vector4f(s.local[0], s.local[1], 0.f, 1.f);
				p.pos[0] = ret[0], p.pos[1] = ret[1];
			}else
				p.pos = s.local;
		};
	};
	
	class ForceOperator : public PSOperator{
	protected:
		Vector2f	mForce;

	public:
		ForceOperator():mForce(0.f,0.f){};

		inline void	set( Vector2f const& force) {
			mForce = force;
		}

		inline void	set( float x, float y) {
			mForce = Vector2f(x,y);
		}

		bool parse(char const* key, lua_State* L);

		template<typename ParticleType>
		inline void update (typename ParticleType::Particle& p,typename ParticleType::State& s){
			s.dPos += s.dLife*mForce;
		};

	};

	/**
	 * velocity operator
	 * generate a range of initial velocity
	 */
	class VelocityOperator : public PSOperator{
	protected:
		float	mAngleStart, mAngleRange;
		float	mVelStart, mVelRange;
		
	public:
		VelocityOperator():mAngleStart(0.f), mAngleRange(2.f*(float)M_PI), 
		mVelStart(0.f),mVelRange(1.f){};
		
		inline void	setAngle( float s, float e) {
			mAngleStart = s;
			mAngleRange = e-s;
		}
		
		inline void	setVelocity( float s, float e) {
			mVelStart = s;
			mVelRange = e-s;
		}
		
		bool parse(char const* key, lua_State* L);
		
		template<class ParticleType>
		inline void spawn(typename ParticleType::Particle& p, typename ParticleType::State& s) const{
			float angle = mAngleStart + mAngleRange * ((float)(rand() & 0xFFFF) / 0xFFFF);
			float vel = mVelStart + mVelRange* ((float)(rand() & 0xFFFF) / 0xFFFF);
			
			s.dPos[0] += cos(angle)*vel;
			s.dPos[1] += sin(angle)*vel;
		};
		
		template<typename ParticleType>
		inline void update (typename ParticleType::Particle& p,typename ParticleType::State& s){
			//s.dPos += s.dVel;
		};
	};
	
	/**
	 * accelerator operator, should follow by velocity
	 * accelerate based on the current velocity
	 * using radical and tangential parameters
	 */
	class AcceleratorOperator : public PSOperator{
	protected:
		float	mRadicalStart, mRadicalRange;
		float	mTanStart, mTanRange;
		
	public:
		AcceleratorOperator():mRadicalStart(0.f), mRadicalRange(0.f), 
		mTanStart(0.f),mTanRange(0.f){};
		
		inline void	setRadical( float s, float e) {
			mRadicalStart = s;
			mRadicalRange = e-s;
		}
		
		inline void	setTan( float s, float e) {
			mTanStart = s;
			mTanRange = e-s;
		}
		
		bool parse(char const* key, lua_State* L);
		
		template<class ParticleType>
		inline void spawn(typename ParticleType::Particle& p, typename ParticleType::State& s) const{
			float rad = mRadicalStart + mRadicalRange * ((float)(rand() & 0xFFFF) / 0xFFFF);
			float tan = mTanStart + mTanRange* ((float)(rand() & 0xFFFF) / 0xFFFF);
			
			s.radical = rad;
			s.tangent = tan;
		};
		
		template<typename ParticleType>
		inline void update (typename ParticleType::Particle& p,typename ParticleType::State& s){
			Vector2f vel = s.dPos;
			if( vel.Normalize() > 0.f ) {
				float tx = -vel[1]*s.tangent, ty = vel[0]*s.tangent;
				s.dPos[0] += vel[0]*s.radical + tx;
				s.dPos[1] += vel[1]*s.radical + ty;
			}
		};
	};
	
}

DECLARE_CLASS(ps::PSSpriteOperator, NilParent);
DECLARE_CLASS(ps::PSTransformOperator, NilParent);
DECLARE_CLASS(ps::ForceOperator, NilParent);
DECLARE_CLASS(ps::VelocityOperator, NilParent);
DECLARE_CLASS(ps::AcceleratorOperator, NilParent);

_CHAOS_END

#endif