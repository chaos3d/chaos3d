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

camera2d& camera2d::operator=(camera2d const& rhs){
    camera::operator=(rhs);
    return *this;
}

void camera2d::collect(const std::vector<game_object *> &goes) {
    const int idx = sprite_mgr::component_idx();
    auto it = goes.begin();
    auto next_sprite = [&] () {
        sprite* spt = nullptr;
        for(;it != goes.end() && spt == nullptr; ++it) {
            spt = (*it)->get_component<sprite>(idx);
        }
        return spt;
    };
    auto* spt = next_sprite();
    auto buffer = spt->index_buffer();
    char* buffer_raw = reinterpret_cast<char*>(buffer->lock());
    
    const void* raw = nullptr;
    size_t raw_size = 0;
    
    std::tie(raw, raw_size) = spt->index_data();
    //buffer->load(raw, 0, raw_size);
    std::memcpy(buffer_raw, raw, raw_size);
    buffer_raw += raw_size;
    size_t offset = raw_size;

    for(sprite* next = next_sprite(); next != nullptr; next = next_sprite()) {        
        if(!next->batchable(*spt)) {
            buffer->unlock();
            spt->generate_batch(target().get(), offset / sizeof(uint16_t));
            
            spt = next;
            
            buffer = spt->index_buffer();
            buffer_raw = reinterpret_cast<char*>(buffer->lock());
            offset = 0;
        } else { // in order to batch, at least ...
            assert(next->index_buffer() == spt->index_buffer()); // index buffer has to be the same
            assert(next->layout() == spt->layout()); // layout needs to be the same
        }
        
        std::tie(raw, raw_size) = next->index_data();
        // TODO: alternative, use lock where applicable like iOS, also test performance
        //buffer->load(raw, offset, raw_size);
        std::memcpy(buffer_raw, raw, raw_size);
        offset += raw_size;
        buffer_raw += raw_size;
    }
    
    buffer->unlock();
    spt->generate_batch(target().get(), offset / sizeof(uint16_t));
    target()->set_batch_retained(false);
}

void camera2d::do_render(const com::render_component_mgr &mgr) {
    target()->do_render(mgr.context());
}