#include "com/sprite2d/sprite.h"
#include "sg/transform.h"
#include "go/game_object.h"
#include "re/vertex_layout.h"
#include "re/render_device.h"
#include "io/memory_stream.h"
#include "re/render_uniform.h"
#include "re/texture.h"

using namespace sprite2d;

#pragma mark - the sprite
sprite::sprite(game_object* go, texture* tex)
: component(go) {
#if 1
    // FIXME with real data
    _data.buffer = sprite_mgr::instance().request_buffer(4);
    //auto tex_uniform = render_uniform::uniform_texture(tex);
    render_uniform::ptr uniform(new render_uniform({
        make_uniform("tex1", tex),
    }));
    _data.material = sprite_mgr::instance().get_material(std::move(uniform));
#if 0
    _data.material = sprite_mgr::instance().get_material(render_uniform::ptr(new render_uniform({
        make_uniform("tex1", tex),
    })));
#endif
#endif
}

sprite::~sprite() {
    sprite_mgr::instance().release_buffer(_data.buffer);
}

void sprite::set_texture(texture *tex) {
    assert(0);
    // TODO: get a new uniform
}

void sprite::generate_batch(render_target *target, size_t batched) const{
    _data.buffer->layout->set_size(batched);
    target->add_batch({
        _data.buffer->layout->retain<vertex_layout const>(),
        std::get<2>(*_data.material),
        std::get<1>(*_data.material),
        std::get<0>(*_data.material)
    });
}

#pragma mark - the manager
sprite_mgr::sprite_mgr(render_device* dev) : _types({
    {{"position", vertex_layout::Float, 4}, {"uv", vertex_layout::Float, 2}},
}), _device(dev){
    assert(dev != nullptr); // needs a device

#if 1 // QUICK TEST
    const char* vs_source = R"shader(
    attribute vec4 position;
    attribute lowp vec2 uv;
    varying lowp vec2 uvVaring;
    void main() {
        gl_Position = position;
        uvVaring = uv;
    }
    )shader";
    
    const char* ps_source = R"shader(
    uniform sampler2D tex1;
    varying lowp vec2 uvVaring;
    
    void main(void)
    {
        gl_FragColor = texture2D(tex1, uvVaring);
    }
    )shader";

    auto vs = std::unique_ptr<gpu_shader>(_device->create_shader(gpu_shader::Vertex));
    vs->compile(make_unique<memory_stream>(vs_source, strlen(vs_source)).get());
    
    auto fs = std::unique_ptr<gpu_shader>(_device->create_shader(gpu_shader::Fragment));
    fs->compile(make_unique<memory_stream>(ps_source, strlen(ps_source)).get());
    
    auto* gpu = _device->create_program();
    gpu->link({"position", "uv"}, {vs.get(), fs.get()});
    
    _materials.emplace_back(gpu, &render_state::default_state());
#endif
}

sprite_mgr::~sprite_mgr() {
    
}

sprite_material* sprite_mgr::get_material(render_uniform::const_ptr const& uniform, int type) {
    assert(type >= 0 && type < _materials.size());
    auto& mat = _materials[type];
    auto it = std::find_if(_sprite_materials.begin(), _sprite_materials.end(), [&] (spt_mat_ptr const& mat) {
        return *std::get<2>(*mat) == (*uniform);
    });
    if(it == _sprite_materials.end()) {
        auto* spt = new sprite_material(std::get<0>(mat), std::get<1>(mat), uniform);
        _sprite_materials.emplace_back(spt);
        return _sprite_materials.back().get();
    }else
        return it->get();
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
        
        auto* transform = it->get_component<com::transform>(transform_idx);
        if(!transform)
            continue; // TODO: log
        
        com->fill_buffer(nullptr, *transform);
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
    
    auto buffer = _device->create_buffer(Vertex_Capacity * offset, vertex_buffer::Dynamic);
    for(auto& it : channels) {
        it.buffer = buffer->retain<vertex_buffer>();
        it.stride = offset;
    }
    offset = (offset + 3) & ~(0x3); // align to multiple of 4 bytes
    
    auto vlayout = _device->create_layout(std::move(channels),
                                           _device->create_index_buffer(Indices_Capacity, vertex_buffer::Stream),
                                           vertex_layout::Triangles);
    return new vertices_buffer({std::move(vlayout), vlayout->index_buffer()});
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
