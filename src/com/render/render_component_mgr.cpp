#include "com/render/render_component_mgr.h"
#include "com/render/camera.h"
using namespace com;

render_component_mgr::render_component_mgr(render_device_ptr device, render_context_ptr context)
: _device(device), _context(context)
{}

render_component_mgr::~render_component_mgr() {
    
}

void render_component_mgr::update(const std::vector<game_object *> & goes) {
    for(auto& it : _cameras) {
        if(it->disabled())
            continue;
        
        it->collect(goes);
        it->do_render(*this);
    }
}

void render_component_mgr::add_camera(camera* cam) {
    assert(std::find_if(_cameras.begin(), _cameras.end(), [=](camera_ptr const& ptr) {
        return ptr.get() == cam;
    }) == _cameras.end());
    _cameras.emplace_front(cam);
    _cameras.sort();
}

void render_component_mgr::remove_camera(camera* cam) {
    _cameras.remove_if([=](camera_ptr const& ptr) {
        return ptr.get() == cam;
    });
}