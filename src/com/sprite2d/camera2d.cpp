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
    auto it = goes.begin();
    auto* spt = (*it)->get_component<sprite>(idx);
    render_batch::batch_t batch;
    for(++it; it != goes.end(); ++it) {
        auto* next = (*it)->get_component<sprite>(idx);
        if(!next)
            continue;
        
        if(next->batchable(*spt)) {
            
        } else {
            //target()->add_batch(const  &)
        }
    }
}

void camera2d::do_render(const com::render_component_mgr &mgr) {
    target()->do_render(mgr.context());
}