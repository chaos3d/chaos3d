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
    vertex_layout* cur = spt->data().buffer->layout;
    vertex_index_buffer* buffer = cur->index_buffer();
    
    const void* raw = nullptr;
    size_t raw_size = 0;
    
    std::tie(raw, raw_size) = spt->indics_buffer();
    buffer->load(raw, 0, raw_size);
    size_t offset = raw_size;

    for(++it; it != goes.end(); ++it) {
        auto* next = (*it)->get_component<sprite>(idx);
        if(!next)
            continue;
        
        if(next->batchable(*spt)) {
            std::tie(raw, raw_size) = spt->indics_buffer();
            buffer->load(raw, offset, raw_size);
            offset += raw_size;
        } else {
            auto& data = spt->data();
            cur->set_size(offset);
            target()->add_batch({
                std::get<1>(*data.material), std::get<2>(*data.material),
                data.buffer->layout, std::get<0>(*data.material)
            });
            
            spt = next;
            cur = spt->data().buffer->layout;
            buffer = cur->index_buffer();
            
            std::tie(raw, raw_size) = spt->indics_buffer();
            offset = raw_size;
            // TODO: alternative, use lock where applicable like iOS, also test performance
            buffer->load(raw, 0, raw_size);
        }
    }
    
    auto& data = spt->data();
    target()->add_batch({
        std::get<1>(*data.material), std::get<2>(*data.material),
        data.buffer->layout, std::get<0>(*data.material)
    });
    target()->set_batch_retained(false);
}

void camera2d::do_render(const com::render_component_mgr &mgr) {
    target()->do_render(mgr.context());
}