// ----------------------------------------------
//
// Vector.h
//
// Created by wuh (2/21)
// Copyright (c) 2009
//
// ----------------------------------------------


#ifndef	WM4VECTOR_H
#define	WM4VECTOR_H

namespace chaos{

	/*
	Vector of 2 elements
	*/
	struct Vector2{
		float	x, y;
	};

	/*
	Vector of 3 elements
	*/
	struct Vector3{
		float	x, y, z;

		
	};

	struct Quaternion{
		float	x, y, z, w;
	};

	/*
	Matrix 4x4

	The elements of the matrix are stored in column-major order
	*/
	template<class Real>
	class Matrix4T{
	protected:
		union{
			Real	_m[16];
			Real	m[4][4];
		};

	public:
        inline Real* operator [] ( const int row ){
			return &_m[row<<2];
        }

		inline Matrix4T(){
			memset( _m, 0, sizeof(_m));
		};

        inline Matrix4T(
            Real m00, Real m01, Real m02, Real m03,
            Real m10, Real m11, Real m12, Real m13,
            Real m20, Real m21, Real m22, Real m23,
            Real m30, Real m31, Real m32, Real m33 )
        {
            m[0][0] = m00;
            m[0][1] = m01;
            m[0][2] = m02;
            m[0][3] = m03;
            m[1][0] = m10;
            m[1][1] = m11;
            m[1][2] = m12;
            m[1][3] = m13;
            m[2][0] = m20;
            m[2][1] = m21;
            m[2][2] = m22;
            m[2][3] = m23;
            m[3][0] = m30;
            m[3][1] = m31;
            m[3][2] = m32;
            m[3][3] = m33;
        };


		static const Matrix4T<Real>		Zero;
	};

	typedef Matrix4T<float>		Matrix4;

};

#endif

