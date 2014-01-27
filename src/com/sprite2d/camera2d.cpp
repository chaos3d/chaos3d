#include "com/sprite2d/camera2d.h"
#include "com/sprite2d/sprite.h"
#include "go/game_object.h"

using namespace sprite2d;

camera2d::camera2d(game_object* go, int priority)
: camera(go, priority)
{}

void camera2d::collect(const std::vector<game_object *> &goes) {
    for(auto it = goes.rbegin(); it != goes.rend(); ++it) {
        auto* spt = (*it)->get_component<sprite>();
        if(!spt)
            continue;
        
    }
}

void camera2d::do_render(const com::render_component_mgr &mgr) {
    
}