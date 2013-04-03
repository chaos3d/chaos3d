// ----------------------------------------------
//
// Rectangle2.h
//
// Created by wuh (3/14)
// Copyright (c) 2009
//
// ----------------------------------------------


#ifndef	WM4RECTANGLE2_H
#define	WM4RECTANGLE2_H

#include "Vector2.h"

namespace Wm4 {

template <class Real>
class  Rectangle
{
public:
	// construction
	Rectangle (); 
	Rectangle ( Real* values ); 
	Rectangle (const Vector2<Real>& rkOrigin,
		Real fExtent0, Real fExtent1);

	void ComputeVertices (Vector2<Real> akVertex[4]) const;

	// corners
	Vector2<Real> GetPPCorner () const;  // O + e0 + e1
	Vector2<Real> GetPMCorner () const;  // O + e0
	Vector2<Real> GetMPCorner () const;  // O + e1
	Vector2<Real> GetMMCorner () const;  // O 

	Vector2<Real> Origin;
	Real Extent[2];
};

#include "Rectangle.inl"

typedef Rectangle<float> Rectanglef;
typedef Rectangle<double> Rectangled;

}

#endif

