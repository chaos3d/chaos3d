#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include "../go/component.h"
#include "Eigen/Geometry"

typedef Eigen::Vector3f vector3f;
typedef Eigen::Affine3f affine3f;
typedef Eigen::Translation3f translation3f;
typedef Eigen::Quaternionf quaternionf;

class transform : public component {
public:
    // transform the position local to this parent to the global space
    vector3f& to_global(vector3f &local) const {
        return local;
    }
    
    // transform from the position global to this parent to the local space
    vector3f& to_local(vector3f &global) const {
        return global;
    }
    
    // update itself using the given parent
    void update(transform const&);
    
    // FIXME: this probably should be in manager class
    // it will backward transverse the parent tree and update
    void force_update();
    
    // keep the global transform, re-compute the local
    void relocate(transform const&);
    
    // local transform piece
    ATTRIBUTE(translation3f, translate);
    ATTRIBUTE(quaternionf, rotate);
    ATTRIBUTE(vector3f, scale);
    
private:
    affine3f _global_affine; // cached global affine transform
};

#endif