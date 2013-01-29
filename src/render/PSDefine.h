/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_PSDEFINE_H
#define	_CHAOS_PSDEFINE_H

#include "chaos_config.h"
#include "ps/ps.h"
#include "PSOperator.h"

_CHAOS_BEGIN

// freely transformation
typedef ParticleSystem< PS_OP_LIST_5(ps::PSSpriteOperator, ps::LifeOperator, ps::ColorOperator, ps::SizeOperator, ps::ForceOperator) >
PS_plcs;
TYPE_RET_REF( PS_plcs );
DECLARE_CLASS_A(PS_plcs, NilParent);

typedef ParticleSystem< PS_OP_LIST_6(ps::PSSpriteOperator, ps::LifeOperator, ps::ColorOperator, ps::SizeOperator, ps::ForceOperator, ps::VelocityOperator) >
PS_plcsv;
TYPE_RET_REF( PS_plcsv );
DECLARE_CLASS_A(PS_plcsv, NilParent);

typedef ParticleSystem< PS_OP_LIST_7(ps::PSSpriteOperator, ps::LifeOperator, ps::ColorOperator, ps::SizeOperator, ps::ForceOperator, ps::AcceleratorOperator, ps::VelocityOperator) >
PS_plcsav;
TYPE_RET_REF( PS_plcsav );
DECLARE_CLASS_A(PS_plcsav, NilParent);

// parent-based transformation
typedef ParticleSystem< PS_OP_LIST_5(ps::PSTransformOperator, ps::LifeOperator, ps::ColorOperator, ps::SizeOperator, ps::ForceOperator) >
PS_tfcsf;
TYPE_RET_REF( PS_tfcsf );
DECLARE_CLASS_A(PS_tfcsf, NilParent);

typedef ParticleSystem< PS_OP_LIST_6(ps::PSTransformOperator, ps::LifeOperator, ps::ColorOperator, ps::SizeOperator, ps::ForceOperator, ps::VelocityOperator) >
PS_tfcsfv;
TYPE_RET_REF( PS_tfcsfv );
DECLARE_CLASS_A(PS_tfcsfv, NilParent);

typedef ParticleSystem< PS_OP_LIST_7(ps::PSTransformOperator, ps::LifeOperator, ps::ColorOperator, ps::SizeOperator, ps::ForceOperator, ps::AcceleratorOperator, ps::VelocityOperator) >
PS_tfcsfav;
TYPE_RET_REF( PS_tfcsfav );
DECLARE_CLASS_A(PS_tfcsfav, NilParent);

#define IMPORT_PS		\
			Declare()	\
				. Class( PS_plcs::__getType() )		\
				. Class( PS_plcsv::__getType() )	\
				. Class( PS_plcsav::__getType() )	

#define IMPORT_PST		\
			Declare()	\
				. Class( PS_tfcsf::__getType() )	\
				. Class( PS_tfcsfv::__getType() )	\
				. Class( PS_tfcsfav::__getType() )	\


_CHAOS_END

#endif

