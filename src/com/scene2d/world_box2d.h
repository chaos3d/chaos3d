#ifndef _WORLD_BOX2D_H
#define _WORLD_BOX2D_H

#include <memory>
#include "go/component_manager.h"
#include "go/component.h"
#include "go/game_object.h"
#include "shape_desc.h"
#include "common/base_types.h"
#include "com/scene2d/world_event.h"

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
        
        collider2d& reset_shapes(std::vector<shape*> const&,
                                 mass const& = mass());
        
        void* internal_data() const; // b2Body
        
        template<class C>
        collider2d& reset_from(bool collidable = false, shape const& = shape());

        vector2f get_velocity() const;
        collider2d& set_velocity(float x, float y);
        
        collider2d& apply_force(vector2f const&);
        collider2d& apply_force(vector2f const&, vector2f const& pos);
        
        collider2d& apply_impulse(vector2f const&);
        collider2d& apply_impulse(vector2f const&, vector2f const& pos);
        
        // TODO:
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
        
        SIMPLE_CLONE(collider2d);
    };
    
    class world2d_mgr : public component_manager_base<world2d_mgr>, public event_dispatcher {
    public:
        typedef std::function<bool(collider2d*)> query_callback_t;
        typedef std::false_type component_fixed_t;
        typedef Eigen::Vector2f vector2f;
        static constexpr float default_pixel_meter_ratio = 0.02f;
        
    public:
        world2d_mgr(float = default_pixel_meter_ratio,
                    vector2f const& gravity = {0.f, -9.81f});
        
        void query(query_callback_t const&,
                   vector2f const& center);
        
        void set_gravity(vector2f const&);
        
    protected:
        virtual void pre_update(goes_t const&) override;
        virtual void update(goes_t const&) override;
        
    private:
        class box2d_listener;
        struct internal;

        std::unique_ptr<internal> _internal;
        std::unique_ptr<box2d_listener> _listener;
        
        ATTRIBUTE(int, velocity_iteration, 6);
        ATTRIBUTE(int, position_iteration, 2);
        ATTRIBUTE(float, step, 1.f/30.f);
        ATTRIBUTE(float, pixel_meter_ratio, 0.02f);
        friend class collider2d;
        friend class box2d_listener;
    };
}

#endif