/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_TIMINGEQUATION_H
#define	_CHAOS_TIMINGEQUATION_H

#include "chaos_config.h"

_CHAOS_BEGIN

/*
Timing equation is used to calculate blend factor beta, which
is in turn used to interpolate between animation key frames.

timing equation is a functor that takes time as input, and
output blend factor beta.
NB because timing is normalized to [0.0, 1.0], input is always
within this range.
*/

// here we implement two basic equations, linear and cubic
// in classes, because classes operator() can be inlined.
class LinearTiming{
public:
	inline float operator()(float time){
		return time;
	};
};

class CubicTiming{
public:
	inline float operator()(float time){
		// http://rechneronline.de/function-graphs/
		// 3*(x)^2*(1-x)+(x)^3
		return time*time*(3-2*time);
	};
};

// if we want a more complex timing function
// then just write a functor

_CHAOS_END

#endif

