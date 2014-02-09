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
    
    class collider2d : public component {
    public:
        typedef world2d_mgr manager_t;
        
        enum { collider_static, collider_dynamic, collider_kinametic };
        enum { type_normal, type_character, type_bullet, };
        
    public:
        collider2d(game_object* go,
                   int = type_normal,
                   int = collider_dynamic);

        void reset_shapes(std::initializer_list<shape::init_t> const&,
                          mass const& = mass());
        
        void* internal_data() const; // b2Body
        
        // update the internal pos from the given transform
        // it will also reset z to 0 and non-z-axies to 0
        void update_from_transform(com::transform &);
        
    protected:
        collider2d& operator=(collider2d const&);
        virtual void destroy() override;
        
        // remove all the shapes bound to the body
        void clear_shapes();
        
        // 
        
    private:
        struct internal;
        std::unique_ptr<internal> _internal;
        
        SIMPLE_CLONE(collider2d);
        friend struct internal;
        
        // TODO
        // mask/category/callback
    };
    
    /// ----------------------------------------------
    /// ----------------------------------------------
    /// ----------------------------------------------

    class world2d_mgr : public component_manager_base<world2d_mgr> {
    public:
        typedef std::false_type component_fixed_t;
        typedef Eigen::Vector2f vector2f;
        static constexpr float pixel_meter_ratio = 0.02f;
        
    public:
        world2d_mgr(float = pixel_meter_ratio,
                    vector2f const& gravity = {0.f, -9.81f});
        
    protected:
        virtual void update(goes_t const&) override;
        
    private:
        struct internal;
        std::unique_ptr<internal> _internal;
        
        friend class collider2d;
    };
}

#endif