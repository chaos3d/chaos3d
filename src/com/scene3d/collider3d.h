#ifndef _CHAOS3D_COM_SCENE3D_COLLIDER3D_H
#define _CHAOS3D_COM_SCENE3D_COLLIDER3D_H

#include "go/component.h"
#include "event/event_dispatcher.h"
#include <memory>

namespace com {
    class transform;
};

namespace scene3d {
    class world3d_mgr;
    
    class collider3d : public component {
    public:
        typedef world3d_mgr manager_t;
        
    public:
        collider3d(game_object*);
        void update_from_transform(com::transform const&);
        
    private:
        struct internal;
        std::unique_ptr<internal> _internal;
    };
    
    class world3d_mgr : public component_manager_base<world3d_mgr>, public event_dispatcher {
    public:
        typedef std::false_type component_fixed_t;
        static constexpr float default_pixel_meter_ratio = 0.02f;

    public:
        world3d_mgr();
        
    protected:
        virtual void pre_update(goes_t const&) override;
        virtual void update(goes_t const&) override;

    private:
        struct internal;
        std::unique_ptr<internal> _internal;
    };
}

#endif
