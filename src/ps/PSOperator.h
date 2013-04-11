/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_PSOPERATOR_H
#define	_CHAOS_PSOPERATOR_H

#include "chaos_config.h"

struct lua_State;

_CHAOS_BEGIN

class PSOperator{
public:
	PSOperator(){};
	inline
	bool parse(char const* key, lua_State* L){ return false; };
	
	inline
	PSOperator&	self(){	return *this;};

	template<class ParticleType>
	inline void update (typename ParticleType::Particle& p,
		typename ParticleType::State const& s){ };

	template<class ParticleType>
	inline void spawn (typename ParticleType::Particle&, 
		typename ParticleType::State&) const{};
	
	inline
	void	finalize () const{};
	
	inline
	void	prepare () const{};
};

_CHAOS_END

#endif

