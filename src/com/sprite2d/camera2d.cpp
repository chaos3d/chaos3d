#include "com/sprite2d/camera2d.h"
#include "com/sprite2d/sprite.h"
#include "com/render/render_component_mgr.h"
#include "go/game_object.h"
#include "re/render_target.h"
#include <utility>

using namespace sprite2d;

camera2d::camera2d(game_object* go, int priority)
: camera(go, priority)
{}

void camera2d::collect(const std::vector<game_object *> &goes) {
    const int idx = sprite_mgr::component_idx();
    auto it = goes.begin();
    auto* spt = (*it)->get_component<sprite>(idx);
    auto buffer = spt->index_buffer();
    
    const void* raw = nullptr;
    size_t raw_size = 0;
    
    std::tie(raw, raw_size) = spt->index_data();
    buffer->load(raw, 0, raw_size);
    size_t offset = raw_size;

    for(++it; it != goes.end(); ++it) {
        auto* next = (*it)->get_component<sprite>(idx);
        if(!next)
            continue;
        
        if(!next->batchable(*spt)) {
            target()->add_batch(spt->associated_batch(offset));
            
            spt = next;
            buffer = spt->index_buffer();
            offset = 0;
        } else { // in order to batch, at least ...
            assert(next->index_buffer() == spt->index_buffer()); // index buffer has to be the same
            assert(next->layout() == spt->layout()); // layout needs to be the same
        }
        
        std::tie(raw, raw_size) = next->index_data();
        // TODO: alternative, use lock where applicable like iOS, also test performance
        buffer->load(raw, offset, raw_size);
        offset += raw_size;
    }
    
    target()->add_batch(spt->associated_batch(offset));
    target()->set_batch_retained(false);
}

void camera2d::do_render(const com::render_component_mgr &mgr) {
    target()->do_render(mgr.context());
}