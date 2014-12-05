#ifndef _CHAOS3D_COM_SCENE3D_COLLIDER3D_H
#define _CHAOS3D_COM_SCENE3D_COLLIDER3D_H

#include "common/base_types.h"
#include "go/component.h"
#include "event/event_dispatcher.h"
#include <memory>

namespace com {
    class transform;
    class camera;
};

namespace scene3d {
    class world3d_mgr;
    
    class collider3d : public component {
    public:
        typedef world3d_mgr manager_t;
        
    public:
        collider3d(game_object*);
        
        /// apply the transform to the physics body
        void update_from_transform(com::transform const&);
        
        /// load shape from the type
        template<typename T>
        collider3d& reset_from();

    protected:
        collider3d& operator=(collider3d const&);
        virtual void destroy() override;

    private:
        struct internal;
        std::unique_ptr<internal> _internal;

        friend class world3d_mgr;
        SIMPLE_CLONE(collider3d);
    };
    
    /// only collision detection for now
    class world3d_mgr : public component_manager_base<world3d_mgr>, public event_dispatcher {
    public:
        typedef std::false_type component_fixed_t;
        static constexpr float default_pixel_meter_ratio = 0.02f;

    public:
        world3d_mgr();

        /// find the collision objects from the view point
        /// in the camera
        void query(com::camera const&, vector2f const& center);
        
    protected:
        virtual void pre_update(goes_t const&) override;
        virtual void update(goes_t const&) override;

    private:
        struct internal;
        std::unique_ptr<internal> _internal;
        
        friend class collider3d;
    };
}

#endif
