#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include "Eigen/Geometry"
#include "go/component.h"
#include "go/component_manager.h"
#include "go/game_object.h"

typedef Eigen::Vector2f vector2f;
typedef Eigen::Vector3f vector3f;
typedef Eigen::Affine3f affine3f;
typedef Eigen::Translation3f translation3f;
typedef Eigen::Quaternionf quaternionf;

namespace com {
    class transform_manager;
    
    class transform : public component {
    public:
        typedef transform_manager manager_t;
        
    public:
        transform(game_object* go)
        : component(go),
        _rotate(1.f, 0.f, 0.f, 0.f),
        _scale(1.f, 1.f, 1.f),
        _translate(0.f, 0.f, 0.f)
        {}

        virtual transform* clone(game_object*) const override;

        // transform the position local to this parent to the global space
        vector3f to_global(vector3f const& local) const {
            return _global_affine * local;
        }
        
        vector3f to_global(vector2f const& local) const {
            return _global_affine * vector3f(local.x(), local.y(), 0.f);
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
        inline bool is_dirty() const;
        
    private:
        quaternionf _rotate;
        vector3f _translate, _scale;
        affine3f _global_affine; // cached global affine transform
        affine3f _global_reversed;
    };
    
    class transform_manager : public component_manager_base<transform_manager> {
    public:
        typedef std::integral_constant<uint32_t, 1> flag_bit_t; // the dirty flag

    public:
        transform_manager();
        
    protected:
        virtual void update(std::vector<game_object*> const&);
        
    private:
        affine3f _global_parent;
    };
    
    inline void transform::mark_dirty() {
        parent()->set_flag(transform_manager::flag_offset());
    }
    
    inline bool transform::is_dirty() const {
        return parent()->is_set(transform_manager::flag_offset());
    }
    
}

#endif