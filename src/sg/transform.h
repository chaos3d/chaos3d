#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include "Eigen/Geometry"
#include "go/component.h"
#include "go/component_manager.h"
#include "go/game_object.h"

typedef Eigen::Vector3f vector3f;
typedef Eigen::Affine3f affine3f;
typedef Eigen::Translation3f translation3f;
typedef Eigen::Quaternionf quaternionf;

namespace com {
    class transform : public component {
    public:
        transform(game_object* go) : component(go)  {}
        
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
        void relocate(game_object*);
        
        // local transform piece
        quaternionf const& rotate() const { return _rotate; }
        vector3f const& translate() const { return _translate; }
        vector3f const& scale() const { return _scale; }
        
        void set_rotate(quaternionf const& rotate) { _rotate = rotate; mark_dirty(); }
        void set_scale(vector3f const& scale) { _scale = scale; mark_dirty(); }
        void set_translate(vector3f const& translate) { _translate = translate; mark_dirty(); }
        
        affine3f const& global() const { return _global_affine; }
        affine3f const& global_reversed() const { return _global_reversed; }
        
        inline void mark_dirty();
    
    private:
        quaternionf _rotate;
        vector3f _translate, _scale;
        affine3f _global_affine; // cached global affine transform
        affine3f _global_reversed;
    };
    
    class transform_manager : public component_manager_base<transform_manager, transform> {
    public:
        typedef std::integral_constant<uint32_t, 1> flag_bit_t; // the dirty flag

    protected:
        virtual void update(std::vector<game_object*> const&);
        
    private:
        affine3f _global_parent;
    };
    
    inline void transform::mark_dirty() {
        parent()->set_flag(transform_manager::flag_offset());
    }

}

#endif