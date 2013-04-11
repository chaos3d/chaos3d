#ifndef _WM4_ALGEBRA_H
#define _WM4_ALGEBRA_H

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include "Matrix2.h"
#include "Matrix3.h"
#include "Matrix4.h"

#include "Quaternion.h"
#include "ColorRGB.h"
#include "ColorRGBA.h"

#include "Rectangle.h"
#include "Rectangle3.h"

#include <cfloat>

#ifndef M_PI
#define M_PI        (3.1415926)
#endif

#ifndef HALF_PI
#define HALF_PI     (M_PI/2.0)
#endif

#define DEGREE_TO_RADIAN(degree)    ((degree)/180.0*M_PI)
#define RADIAN_TO_DEGREE(radian)    ((radian)/M_PI*180.0)

#endif