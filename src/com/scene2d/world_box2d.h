#ifndef _WORLD_BOX2D_H
#define _WORLD_BOX2D_H

#include <memory>
#include <Eigen/Dense>
#include "go/component_manager.h"
#include "go/component.h"
#include "go/game_object.h"

namespace scene2d {
    class world2d_mgr;
    
    class collider2d : public component {
    public:
        typedef world2d_mgr manager_t;
        
    public:
        collider2d(game_object* go);
        
        virtual void destroy() override;
       
    protected:
        collider2d& operator=(collider2d const&);
        
    private:
        struct internal;
        std::unique_ptr<internal> _internal;
        
        SIMPLE_CLONE(collider2d);
        friend struct internal;
    };
    
    class world2d_mgr : public component_manager_base<world2d_mgr> {
    public:
        typedef std::false_type component_fixed_t;
        typedef Eigen::Vector2f vector2f;
    public:
        world2d_mgr(vector2f const& gravity = {0.f, -9.81f});
        
    protected:
        virtual void update(goes_t const&) override;
        
    private:
        struct internal;
        std::unique_ptr<internal> _internal;
        
        friend class collider2d;
    };
}

#endif