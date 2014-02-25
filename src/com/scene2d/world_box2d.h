#ifndef _WORLD_BOX2D_H
#define _WORLD_BOX2D_H

#include <memory>
#include "go/component_manager.h"
#include "go/component.h"
#include "go/game_object.h"
#include "shape_desc.h"
#include "common/base_types.h"

namespace com {
    class transform;
}

namespace scene2d {
    class world2d_mgr;
    
    // the 2d collider that interacts with phiysics world
    // it will remove non-z-axie rotation
    // and reset scaling to 1.0
    class collider2d : public component {
    public:
        typedef world2d_mgr manager_t;
        
        enum { static_, dynamic, kinametic };
        enum { type_normal, type_character, type_bullet, };
        
    public:
        collider2d(game_object* go,
                   int = static_,
                   int = type_normal);

        collider2d& reset_shapes(std::initializer_list<shape::init_t> const&,
                                 mass const& = mass());
        
        void* internal_data() const; // b2Body
        
        template<class C>
        collider2d& reset_from(bool collidable = false, shape const& = shape());
        
        // TODO:
        // apply force/impulse
        // get velocity
        // other body settings
        // etc.
    protected:
        collider2d& operator=(collider2d const&);
        virtual void destroy() override;
        
        // remove all the shapes bound to the body
        void clear_shapes();
        
        // update the internal pos from the given transform
        // it will also reset non-z-axies to 0 and remove scaling
        void update_from_transform(com::transform &, float ratio);
        
        // apply the changes to the transform
        void apply_to_transform(com::transform &, float ratio) const;
        
    private:
        struct internal;
        std::unique_ptr<internal> _internal;
        
        friend struct internal;
        friend class world2d_mgr; //updating transform
        
        // TODO
        // mask/category/callback
        SIMPLE_CLONE(collider2d);
    };
    
    /// ----------------------------------------------
    /// ----------------------------------------------
    /// ----------------------------------------------

    class world2d_mgr : public component_manager_base<world2d_mgr> {
    public:
        typedef std::function<bool(collider2d*)> query_callback_t;
        typedef std::false_type component_fixed_t;
        typedef Eigen::Vector2f vector2f;
        static constexpr float default_pixel_meter_ratio = 0.02f;
        
    public:
        world2d_mgr(float = default_pixel_meter_ratio,
                    vector2f const& gravity = {0.f, -9.81f});
        
        void query(query_callback_t const&,
                   vector2f const& center,
                   vector2f const& half_extent = {FLT_EPSILON, FLT_EPSILON});
    protected:
        virtual void pre_update(goes_t const&) override;
        virtual void update(goes_t const&) override;
        
    private:
        struct internal;
        std::unique_ptr<internal> _internal;
        
        ATTRIBUTE(int, velocity_iteration, 6);
        ATTRIBUTE(int, position_iteration, 2);
        ATTRIBUTE(float, step, 1.f/30.f);
        ATTRIBUTE(float, pixel_meter_ratio, 0.02f);
        friend class collider2d;
    };
}

#endif