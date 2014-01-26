#include "com/render/render_component_mgr.h"
#include "com/render/camera.h"

using namespace com;

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
    assert(std::find_if(_cameras.begin(), _cameras.end(), [=](std::unique_ptr<camera> const& ptr) {
        return ptr.get() == cam;
    }) == _cameras.end());
    _cameras.emplace_back(cam);
}
