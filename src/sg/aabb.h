#ifndef _AABB_H
#define _AABB_H

#include "Eigen/Geometry"
#include "../go/component.h"

typedef Eigne::AlignedBox3f aligned_box3f;

class aabb : public component {
private:
    aligned_box3f _box;
    
    // the manager will have parent aabb and
    // clipping algorithms
};
#endif