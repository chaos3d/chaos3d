#include "com/sprite2d/camera2d.h"
#include "com/sprite2d/sprite.h"
#include "com/render/render_component_mgr.h"
#include "go/game_object.h"
#include "re/render_target.h"

using namespace sprite2d;

camera2d::camera2d(game_object* go, int priority)
: camera(go, priority)
{}

void camera2d::collect(const std::vector<game_object *> &goes) {
    const int idx = sprite_mgr::component_idx();
    for(auto it = goes.rbegin(); it != goes.rend(); ++it) {
        auto* spt = (*it)->get_component<sprite>(idx);
        if(!spt)
            continue;
        
    }
}

void camera2d::do_render(const com::render_component_mgr &mgr) {
    target()->do_render(mgr.context());
}