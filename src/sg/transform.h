#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include "go/component.h"
#include "Eigen/Geometry"
#include "go/component_manager.h"

typedef Eigen::Vector3f vector3f;
typedef Eigen::Affine3f affine3f;
typedef Eigen::Translation3f translation3f;
typedef Eigen::Quaternionf quaternionf;

namespace com {
    class transform : public component {
    public:
        transform(game_object* go) : component(go) {}
        
        // transform the position local to this parent to the global space
        vector3f to_global(vector3f const& local) const {
            return _global_affine * local;
        }
        
        // transform from the position global to this parent to the local space
        vector3f to_local(vector3f const& global) const {
            return _global_reversed * global;
        }
        
        // update itself using the given parent
        void update(affine3f const&);
        
        // FIXME: this probably should be in manager class
        // it will backward transverse the parent tree and update
        void force_update();
        
        // keep the global transform, re-compute the local
        void relocate(transform const&);
        
        // local transform piece
        ATTRIBUTE(vector3f, translate);
        ATTRIBUTE(quaternionf, rotate);
        ATTRIBUTE(vector3f, scale);
        
        affine3f const& global() const { return _global_affine; }
        
    private:
        affine3f _global_affine; // cached global affine transform
        affine3f _global_reversed;
    };
    
    class transform_manager : public component_manager_base<transform> {
    protected:
        virtual void update(std::vector<game_object*> const&);
        
    private:
        affine3f _global_parent;
    };
}
#endif