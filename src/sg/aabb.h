#ifndef _AABB_H
#define _AABB_H

#include <Eigen/Geometry>
#include "go/component.h"


class aabb : public component {
public:
    typedef Eigne::AlignedBox3f aabb3f;

private:
    aabb3f _box;
    
    // the manager will have parent aabb and
    // clipping algorithms
};
#endif