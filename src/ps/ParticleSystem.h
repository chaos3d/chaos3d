/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_PARTICLESYSTEM_H
#define	_CHAOS_PARTICLESYSTEM_H

#include "chaos_config.h"
#include "core/RTTI.h"
#include "core/math.h"
#include "anim/anim.h"
#include "script/lua.h"

#include "ParticleGenericOperator.h"
#include "ParticleEmitterOperator.h"

#define	PS_OP_LIST_1(T1)						PSOperatorList<T1, PSNilOperator>
#define	PS_OP_LIST_2(T1,T2)						PSOperatorList<T1, PSOperatorList<T2, PSNilOperator> > 
#define	PS_OP_LIST_3(T1,T2,T3)					PSOperatorList<T1, PSOperatorList<T2, PSOperatorList<T3, PSNilOperator> > >
#define	PS_OP_LIST_4(T1,T2,T3,T4)				PSOperatorList<T1, PSOperatorList<T2, PSOperatorList<T3, PSOperatorList<T4, PSNilOperator> > > > 
#define	PS_OP_LIST_5(T1,T2,T3,T4,T5)			PSOperatorList<T1, PSOperatorList<T2, PSOperatorList<T3, PSOperatorList<T4, PSOperatorList<T5, PSNilOperator> > > > > 
#define	PS_OP_LIST_6(T1,T2,T3,T4,T5,T6)			PSOperatorList<T1, PSOperatorList<T2, PSOperatorList<T3, PSOperatorList<T4, PSOperatorList<T5, PSOperatorList<T6, PSNilOperator> > > > > >
#define	PS_OP_LIST_7(T1,T2,T3,T4,T5,T6,T7)		PSOperatorList<T1, PSOperatorList<T2, PSOperatorList<T3, PSOperatorList<T4, PSOperatorList<T5, PSOperatorList<T6, PSOperatorList<T7, PSNilOperator> > > > > > > 
#define	PS_OP_LIST_8(T1,T2,T3,T4,T5,T6,T7,T8)	PSOperatorList<T1, PSOperatorList<T2, PSOperatorList<T3, PSOperatorList<T4, PSOperatorList<T5, PSOperatorList<T6, PSOperatorList<T7, PSOperatorList<T8, PSNilOperator> > > > > > > >

_CHAOS_BEGIN

/*
struct ParticleType{
	struct Particle{
		ColorRGBA	color;
		Vector3f	pos;		// dpos == velocity
		float		size;
		float		life;
	};
	struct State{
		Vector3f	pos;		// original position
		ColorRGBA	dColor;
		Vector3f	dPos;		// dpos == velocity
		float		dSize;
		float		dLife;
	};
};
*/
//template<class Particle>
class PSNilOperator{
public:
//	inline PSNilOperator&	self(){	return *this;};
	PSNilOperator() {};

	template<class ParticleType>
	inline void update (typename ParticleType::Particle const&,
		typename ParticleType::State const&){ };

	template<class ParticleType>
	inline void		spawn (typename ParticleType::Particle const&, 
		typename ParticleType::State&) const{};

	inline void		finalize () const{};
	inline void		prepare () const{};
	inline bool parse(char const* key, lua_State* L){ return false;};
};

template< class Head, class Tail>
class PSOperatorList{
protected:
	Head	mHead;
	Tail	mTail;

public:
	PSOperatorList() {};
	inline Head&	self(){	return mHead;	};
	inline Tail&	next(){	return mTail;	};

	template<class ParticleType>
	inline void update (typename ParticleType::Particle& p,
		typename ParticleType::State& s){
			mHead.update<ParticleType>( p, s); 
			mTail.update<ParticleType>( p, s);
	};

	template<class ParticleType>
	inline void spawn (typename ParticleType::Particle& p, 
		typename ParticleType::State& s){
			mHead.spawn<ParticleType>( p, s );
			mTail.spawn<ParticleType>( p, s );
	};

	inline void	finalize ()				{ mHead.finalize( );	mTail.finalize( );	};
	inline void	prepare ()				{ mHead.prepare( ); mTail.prepare( );	};

	inline bool parse(char const* key, lua_State* L){
		int n = lua_gettop(L);
		bool ret = mHead.parse( key, L );
		lua_settop(L, n);
		return ret || mTail.parse(key,L);
	};
};

template<class OPList, unsigned int idx>
struct OperatorAt;

template<class Head, class Tail>
struct OperatorAt<PSOperatorList<Head, Tail>, 0>{
   typedef Head Result;
   inline Result&	operator_(PSOperatorList<Head, Tail>& oplist){
	   return oplist.self();
   }
};

template <class Head, class Tail, unsigned int idx>
struct OperatorAt<PSOperatorList<Head, Tail>, idx>{
   typedef typename OperatorAt<Tail, idx - 1>::Result Result;
   inline Result&	operator_(PSOperatorList<Head, Tail>& oplist){
	   return OperatorAt<Tail, idx - 1 >().operator_(oplist.next());
   }
};

template<class Operator>
class ParticleSystem;

template<class ParticleType, class Operator, class Renderer>
class CHAOS_API ParticleAnimationState : public AnimationState{
public:
	typedef ParticleSystem<Operator>	TPS;
	typedef typename ParticleType::Particle	Particle;
	typedef typename ParticleType::State	State;

protected:
	TPS*		mPS;
	Renderer*	mRenderer;
	int			mCount;

public:
	ParticleAnimationState(TPS* ps, Renderer* rd)
		: mPS(ps), mCount(0), mRenderer(rd)
	{
		ASSERT( ps != 0 && rd != 0 );
		if( ps != 0 ) ps->retain();
		//FIX ME:
		rd->emit(mCount,0);
	};

	~ParticleAnimationState(){
		if( mPS != 0 ) 
			mPS->release();
	};

	void	spawn(int c){
		c = mRenderer->emit(mCount, c);
		//if( mCount + c >= mCapacity )
		//	c = mCapacity - mCount;

		Particle* p = mRenderer->getParticle() + mCount;
		State* s = mRenderer->getState() + mCount;
		for( ; mCount<c; ++mCount )
			mPS->getOp().template spawn<ParticleType>( *p ++, *s ++);
	};

	virtual bool update(float t, int c){
		spawn(c);

		Particle* p = mRenderer->getParticle();
		Particle* pe = mRenderer->getParticle() + mCount;
		State* s = mRenderer->getState();
		State* se = mRenderer->getState() + mCount;
		mPS->getOp().prepare();

		int dead(0);
		for( int i=0; i<mCount; ++i ){
			if( p->life < 0.f ){
				*p = *--pe;
				*s = *--se;
				++dead;
			}else{
				mPS->getOp().template update<ParticleType>( *p, *s );
				++p, ++s;
			}
		}
		if( dead > 0 ){
			mCount -= dead;
			mRenderer->emit(mCount, 0);
		}
		mPS->getOp().finalize();

		//mRenderer.render( mParticles, mCount );
		return mCount > 0;
	};

	virtual void stop(float count=0.f){
		// FIX: only temparory
		mCount = 0;
		mRenderer->emit(0,0);
		AnimationState::stop(count);
	};
};

template<class Operator>
class CHAOS_API ParticleSystem : public ReferencedCount{
protected:
	Operator	mOp;

	/*
	// helper function for script
	template<class Particle, class RendererWrapper>
	AnimationState*	animation(typename RendererWrapper::Renderee* rdee, int cap = 0){
		typedef typename RendererWrapper::Renderer Renderer;
		if( cap == 0 )	cap = 64;
		ParticleAnimationState<Particle,Operator,Renderer>* pa
			= new ParticleAnimationState<Particle,Operator,Renderer>(this, Renderer(rdee), cap);
		pa->autorelease();
		return pa;			
	}*/
public:
	ParticleSystem(TypeLua const& lua){
		lua_State* L(lua.getL());
		int n = lua_gettop( L );
		do{
			lua.get(L);
			if( !lua_istable( L, -1 ) ){
				LOG("Expected a table. (ParticleSystem)");
				break;
			}
			lua_pushnil( L );
			while( lua_next( L, -2 ) ){
				int n = lua_gettop(L);
				if( lua_type(L,-2) == LUA_TSTRING ){
					char const *key = lua_tostring(L, -2);
					if( !mOp.parse( key, L ) )
						LOG("Ignore the setting (%s). (ParticleSystem)", key);
				}
				lua_settop(L, n-1);
			}
		}while(0);
		lua_settop(L, n);
	}

	ParticleSystem(){}

	~ParticleSystem(){
	};

	inline Operator&	getOp() { return mOp; };

	template<unsigned int idx>
	typename OperatorAt<Operator, idx>::Result&
		operator_(){
		return OperatorAt<Operator, idx>().operator_(mOp);
	}

	template<class ParticleType, class Renderer>
	//ParticleAnimationState<ParticleType,Operator,Renderer>*
	AnimationState *createState(Renderer* rd)
	{
		ParticleAnimationState<ParticleType,Operator,Renderer>* pa
			= new ParticleAnimationState<ParticleType,Operator,Renderer>(this, rd);
		pa->autorelease();
		return pa;			
	}

};

/*
typedef ParticleSystem< PS_OP_LIST_5(ps::PointEmitter, ps::LifeOperator, ps::ColorOperator, ps::SizeOperator, ps::ForceOperator) >
	PA_plcs;

TYPE_RET_REF( PA_plcs );

DECLARE_SPEC( PA_plcs );
*/
_CHAOS_END

#endif
