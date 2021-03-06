#ifndef _CHAOS3D_COM_RENDER_CAMERA_MGR_H
#define _CHAOS3D_COM_RENDER_CAMERA_MGR_H

#include "go/component.h"
#include "go/component_manager.h"
#include "re/render_device.h"
#include "re/render_context.h"
#include <vector>
#include <forward_list>
#include <memory>

namespace com {
    class camera;
    class renderable;
    
    class camera_mgr : public component_manager_base<camera_mgr> {
    public:
        typedef std::unique_ptr<com::camera, component::component_deleter> camera_ptr;
        typedef std::forward_list<camera_ptr> cameras_t;
        typedef std::vector<std::unique_ptr<renderable>> renderables_t;
        typedef render_device* render_device_ptr;
        typedef render_context* render_context_ptr;

    public:
        camera_mgr(render_device_ptr, render_context_ptr);
        ~camera_mgr();
        
        void add_camera(camera*);
        void remove_camera(camera*);

        render_context_ptr context() const { return _context; }
        render_device_ptr device() const { return _device; }
        
    protected:
        virtual void update(std::vector<game_object*> const&) override;
        
        void add_renderable(renderable*);
    private:
        render_device_ptr _device;
        render_context_ptr _context;
        cameras_t _cameras;

        friend class renderable;
    };
}
#endif