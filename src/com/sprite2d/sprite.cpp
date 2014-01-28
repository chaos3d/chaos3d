#include "com/sprite2d/sprite.h"
#include "sg/transform.h"
#include "go/game_object.h"
#include "re/vertex_layout.h"
#include "re/render_device.h"

using namespace sprite2d;

#pragma mark - the sprite
sprite::sprite(game_object* go)
: component(go) {
    _data.buffer = sprite_mgr::instance().request_buffer(4);
}

sprite::~sprite() {
    sprite_mgr::instance().release_buffer(_data.buffer);
}

void sprite::fill_buffer(com::transform* transform) {
    assert(transform != nullptr); // needs transform component
    
    transform->global();
}

#pragma mark - the manager
sprite_mgr::sprite_mgr(render_device* dev) : _types({
    {{"position", vertex_layout::Float, 4}, {"uv", vertex_layout::Float, 2}},
}), _device(dev){
    assert(dev != nullptr); // needs a device
}

void sprite_mgr::update(goes_t const& gos) {
    auto idx = component_idx();
    auto transform_idx = com::transform_manager::component_idx();
    for(auto& it : gos) {
        if(!it->is_set(flag_offset()))
            continue;
        
        auto* com = it->get_component<sprite>(idx);
        if(!com)
            continue;
        
        com->fill_buffer(it->get_component<com::transform>(transform_idx));
    }
}

vertices_buffer* sprite_mgr::create_buffer(vertices_t const& layout) {
    vertex_layout::channels_t channels;
    channels.reserve(layout.size());
    
    size_t offset = 0;
    for(auto& it : layout) {
        channels.push_back({nullptr, it.type, it.count, offset, 0});
        offset += vertex_layout::type_size(it.type) * it.count;
    }
    
    auto* buffer = _device->create_buffer(Vertex_Capacity * offset, vertex_buffer::Dynamic);
    for(auto& it : channels) {
        it.buffer = buffer;
        it.stride = offset;
    }
    
    auto* indices = _device->create_index_buffer(Indices_Capacity, vertex_buffer::Stream);
    auto* vlayout = _device->create_layout(channels, vertex_layout::Triangles, indices);
    return new vertices_buffer({vlayout, indices});
}

vertices_buffer* sprite_mgr::request_buffer(uint32_t count, int type_idx) {
    assert(type_idx >= 0 && type_idx < _types.size());
    assert(0);
    return nullptr;
}

void sprite_mgr::release_buffer(vertices_buffer*) {
    assert(0);
}

uint8_t sprite_mgr::add_type(vertices_t const& layout) {
    auto it = std::find(_types.begin(), _types.end(), layout);
    if(it != _types.end())
        return it - _types.begin();
    _types.push_back(layout);
    std::sort(_types.back().begin(), _types.back().end(), [] (sprite_vertex const& lhs,
                                                              sprite_vertex const& rhs) {
        return lhs.name < rhs.name;
    });
    
    // no identical name
    assert(std::adjacent_find(_types.back().begin(), _types.back().end(),[] (sprite_vertex const& lhs,
                                                                             sprite_vertex const& rhs) {
        return lhs.name == rhs.name;
    }) == _types.back().end());
    return _types.size() - 1;
}
