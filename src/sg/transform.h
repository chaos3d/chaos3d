#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include "Eigen/Geometry"
#include "go/component.h"
#include "go/component_manager.h"
#include "go/game_object.h"

typedef Eigen::Vector2f vector2f;
typedef Eigen::Vector3f vector3f;
typedef Eigen::Affine3f affine3f;
typedef Eigen::Matrix3f matrix3f;
typedef Eigen::Translation3f translation3f;
typedef Eigen::Quaternionf quaternionf;

namespace com {
    class transform_manager;
    
    class transform : public component {
    public:
        typedef transform_manager manager_t;
        
    public:
        transform(game_object* go,
                  vector3f const& translate = {0.f,0.f,0.f},
                  quaternionf const& rotate = {1.f, 0.f, 0.f, 0.f},
                  vector3f const& scale = {1.f,1.f,1.f});

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
            return _global_inverse * global;
        }
        
        // update the global using the given parent
        // that is, to keep the local transform
        void update_global(affine3f const*);
        
        // update the local using the given parent (transform inverse)
        // that is, to keep the global transform
        void update_local(affine3f const* /*transform inverse*/);
        
        // FIXME: need to update local but not populate to its children
        void relocate();
        
        // backward transverse the parent tree and update the matrix
        // it will also populate the flags and only reset itself so
        // it won't get computed again
        // TODO: this will be expensive or asynchroneously?
        transform& force_update();
        
        //affine3f const& global() const { return _global_affine; }
        affine3f const& global_inverse() const { return _global_inverse; }
        
        // to update global or local
        // if both has been marked, to update global takes priorities
        // (keep the local transform)
        inline transform& mark_dirty(bool global = true);
        inline bool is_dirty() const;
        
        // helper function to set rotation
        transform& set_rotate_by_axis(float x, float y, float z);
        
        /// set the skew angel in degrees for x-axis and y-axis
        transform& set_skew(float angle_x, float angle_y);
        
    private:
        affine3f _global_inverse;
        
        ATTRIBUTE(affine3f, global_affine, affine3f::Identity());
        ATTRIBUTE(quaternionf, rotate, quaternionf(1.f, 0.f, 0.f, 0.f));
        ATTRIBUTE(vector3f, translate, vector3f(0.f, 0.f, 0.f));
        ATTRIBUTE(vector3f, scale, vector3f(1.f, 1.f, 1.f));
        
        /// the skew (tangent) for x-axis (first) and y-axis on xy-plane
        /// this is 'hard' to retrieve from the matrix so they will be
        /// ditched for the given matrix
        ATTRIBUTE(vector3f, skew, vector3f(0.f, 0.f, 0.f));
    };
    
    class transform_manager : public component_manager_base<transform_manager> {
    public:
        // the dirty flag, to update global or local
        typedef std::integral_constant<uint32_t, 2> flag_bit_t;
        
        constexpr static uint32_t global_bit = 1U;
        constexpr static uint32_t local_bit = 2U;
        constexpr static uint32_t mask_bit = 3U; // two bits

    public:
        transform_manager();
        
    protected:
        virtual void update(std::vector<game_object*> const&);
        
    private:
        affine3f _global_parent;
    };
    
    inline transform& transform::mark_dirty(bool global) {
        parent()->set_flag(transform_manager::flag_offset(),
                           global ? transform_manager::global_bit :
                           transform_manager::local_bit);
        return *this;
    }
    
    inline bool transform::is_dirty() const {
        return ((parent()->flag() >> transform_manager::flag_offset()) & 0x3U) != 0U;
    }
}

#endif