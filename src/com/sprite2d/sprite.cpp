#include "com/sprite2d/sprite.h"
#include "sg/transform.h"
#include "go/game_object.h"
#include "re/render_device.h"
#include "io/memory_stream.h"
#include "re/texture.h"

using namespace sprite2d;

#pragma mark - the sprite
sprite::sprite(game_object* go, size_t count, int type)
: component(go), _mark_for_remove(false) {
    // always give away the ownership to the manager
    sprite_mgr::instance().assign_buffer(this, count, type);
#if 1
    // FIXME with real data
    //_data.buffer = sprite_mgr::instance().request_buffer(4);
    //auto tex_uniform = render_uniform::uniform_texture(tex);
//    render_uniform::ptr uniform(new render_uniform({
//        make_uniform("tex1", tex),
//    }));
//    _data.material = sprite_mgr::instance().get_material(std::move(uniform));
#if 0
    _data.material = sprite_mgr::instance().get_material(render_uniform::ptr(new render_uniform({
        make_uniform("tex1", tex),
    })));
#endif
#endif
}

sprite::~sprite() {
}

void sprite::destroy() {
    _mark_for_remove = true;
    assert(0); // FIXME: delete this in sprite mgr
}

void sprite::generate_batch(render_target *target, size_t batched) const{
    _data.buffer->layout->set_size(batched);
    target->add_batch(
        _data.buffer->layout->retain<vertex_layout const>(),
        _data.material->uniform(),
        _data.material->state(),
        _data.material->program()->retain<gpu_program const>()
    );
}

#pragma mark - sprite material
sprite_material sprite_material::set_uniforms(const std::initializer_list<render_uniform::init_t> &list,
                                              const render_state::ptr & state_new) const{
    // FIXME: test intercetion first?
    render_uniform::ptr uniform_new(new render_uniform(*_uniform));
    uniform_new->merge(list, false);
    return sprite_material(_program->retain<gpu_program>(),
                           state_new.get() == nullptr ? _state : state_new,
                           std::move(uniform_new));
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

    auto vs = _device->create_shader(gpu_shader::Vertex);
    vs->compile(make_unique<memory_stream>(vs_source, strlen(vs_source)).get());
    
    auto fs = _device->create_shader(gpu_shader::Fragment);
    fs->compile(make_unique<memory_stream>(ps_source, strlen(ps_source)).get());
    
    auto gpu = _device->create_program();
    gpu->link({"position", "uv"}, {vs.get(), fs.get()});
    
    _materials.emplace_back(gpu->retain<gpu_program>(), render_state::default_state());
#endif
}

sprite_mgr::~sprite_mgr() {
    
}

sprite_material* sprite_mgr::get_material(render_uniform::const_ptr const& uniform, int type) {
    assert(type >= 0 && type < _materials.size());
    auto& mat = _materials[type];
    auto it = std::find_if(_sprite_materials.begin(), _sprite_materials.end(), [&] (spt_mat_ptr const& mat) {
        return *mat->_uniform == (*uniform);
    });
    if(it == _sprite_materials.end()) {
        assert(0);
        sprite_material* spt = nullptr;//new sprite_material(std::get<0>(mat)->retain<gpu_program const>(), std::get<1>(mat), uniform);
        _sprite_materials.emplace_back(spt);
        return _sprite_materials.back().get();
    }else
        return it->get();
}

void sprite_mgr::update(goes_t const& gos) {
    auto transform_idx = com::transform_manager::component_idx();
    
    //auto mark = gos.front()->mark();
    // step 1: update all the indices, remove invisible or deleted sprites
    for(auto& it : _buffers) {
        for(auto& sprite : it.sprites) {
            auto* spt = std::get<0>(sprite);
            if(spt->_mark_for_remove
               //|| spt->parent()->mark() != mark // only move sprites when it gets deleted?
               ) {
                // TODO: remove, update index
            }
        }
    }

    // step 2: update the position if needed
    auto offset = flag_offset();
    for(auto& it : _buffers) {
        // uses the first buffer
        // TODO: asynch locking?
        void* buffer = it.layout->channels()[0].buffer.get()->lock();
        auto stride = it.layout->channels()[0].stride;
        for(auto& sprite : it.sprites) {
            auto* spt = std::get<0>(sprite);

            auto* transform = spt->parent()->get_component<com::transform>(transform_idx);
            if(!transform)
                continue; // TODO: log

            if(spt->parent()->is_set(offset)) {
                spt->fill_buffer(buffer, stride, *transform);
            }
        }
    }
}

layout_buffer* sprite_mgr::create_buffer(vertices_t const& layout) {
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
    return new layout_buffer({std::move(vlayout), {}, 0});
}

void sprite_mgr::assign_buffer(sprite*, uint32_t count, int typeIdx) {
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
