#ifndef _RENDER_TYPES_H
#define _RENDER_TYPES_H

#include "Eigen/Dense"

class texture {
public:
    typedef Eigen::Vector2f vector2f;
    enum { T1D, T2D, T3D };

    virtual ~texture() {};
    
    vector2f const& size() const;
    int type() const;
    int format() const;
};


#endif