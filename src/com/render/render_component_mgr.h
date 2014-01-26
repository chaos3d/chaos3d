#ifndef _RENDER_COMPONENT_MGR_H
#define _RENDER_COMPONENT_MGR_H

#include "go/component_manager.h"
#include "re/render_device.h"
#include <vector>
#include <memory>

namespace com {
    class camera;
    class renderable;
    
    class render_component_mgr : public component_manager_base<render_component_mgr> {
    public:
        typedef std::vector<std::unique_ptr<com::camera>> cameras_t;
        typedef std::vector<std::unique_ptr<renderable>> renderables_t;
        typedef render_device* render_device_ptr;

    public:
        ~render_component_mgr();
        
    protected:
        virtual void update(std::vector<game_object*> const&) override;
        
        void add_camera(camera*);
        void add_renderable(renderable*);
        
    private:
        render_device_ptr _device;
        cameras_t _cameras;

        //
        friend class camera;
        friend class renderable;
    };
}
#endif