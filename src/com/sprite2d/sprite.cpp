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
    _data.buffer = sprite_mgr::instance().assign_buffer(this, count, type);
}

sprite::~sprite() {
}

void sprite::destroy() {
    _mark_for_remove = true;
}

void sprite::generate_batch(render_target* target, size_t start, size_t count) const {
    assert(_data.buffer != nullptr && _data.material != nullptr);
    target->add_batch(_data.buffer->layout->retain<vertex_layout const>(),
                      _data.material->uniform(),
                      _data.material->state(),
                      _data.material->program()->retain<gpu_program const>(),
                      start,
                      count);
}

bool sprite::set_material(const std::string & name,
                          std::initializer_list<render_uniform::init_t> const& list,
                          render_state::ptr const& state) {
    auto* mat = sprite_mgr::instance().find_first_material(name);
    if(mat == nullptr)
        return false;
    _data.material = mat;
    mod_material(list, state);
    return true;
}

void sprite::mod_material(std::initializer_list<render_uniform::init_t> const& list,
                          render_state::ptr const& state) {
    auto* mat = _data.material;
    if(!mat || (list.size() == 0 && state.get() == nullptr))
        return; // TODO: log
    
    auto mat_new = make_unique<sprite_material>(mat->set_uniforms(list, state));
    
    // TODO: test if it is changed?
    set_material(sprite_mgr::instance().add_material(std::move(mat_new)));
}

#pragma mark - sprite material
sprite_material sprite_material::set_uniforms(const std::initializer_list<render_uniform::init_t> &list,
                                              const render_state::ptr & state_new) const{
    // FIXME: test intercetion first?
    render_uniform::ptr uniform_new(new render_uniform(*_uniform));
    uniform_new->merge(render_uniform(list), false);
    return sprite_material(name(),
                           _program->retain<gpu_program>(),
                           state_new.get() == nullptr ? _state : state_new,
                           std::move(uniform_new));
}

#pragma mark - the manager
sprite_mgr::sprite_mgr(render_device* dev, size_t vsize, size_t isize,
                       std::array<std::string, layout_buffer::MAX> const& channels)
: _types({
    {
        {channels[layout_buffer::POSITION], vertex_layout::Float, 4},
        {channels[layout_buffer::UV], vertex_layout::Float, 2}
    },
}), _device(dev), _vertex_buffer_size(vsize), _index_buffer_size(isize),
_channel_names(channels){
    assert(dev != nullptr); // needs a device

#if 1 // QUICK TEST
    const char* vs_source = R"shader(
    attribute vec4 position;
    attribute lowp vec2 uv;
    uniform mat4 c_ProjViewMat;
    varying lowp vec2 uvVaring;
    varying lowp float alpha;
    void main() {
        gl_Position = c_ProjViewMat * vec4(position.xyz, 1.);
        uvVaring = uv;
        alpha = position.w;
    }
    )shader";
    
    const char* ps_source = R"shader(
    uniform sampler2D tex1;
    varying lowp vec2 uvVaring;
    varying lowp float alpha;
    
    void main(void)
    {
        gl_FragColor = texture2D(tex1, uvVaring) * vec4(1.0, 1.0, 1.0, alpha);
    }
    )shader";

    auto vs = _device->create_shader(gpu_shader::Vertex);
    vs->compile(make_unique<memory_stream>(vs_source, strlen(vs_source)).get());
    
    auto fs = _device->create_shader(gpu_shader::Fragment);
    fs->compile(make_unique<memory_stream>(ps_source, strlen(ps_source)).get());
    
    auto gpu = _device->create_program();
    gpu->link(vertex_layout(0), {vs.get(), fs.get()});
    
    render_state state;
#if 0
    state.set_src_alpha_blend(render_state::BlendSrcAlpha);
    state.set_src_blend(render_state::BlendSrcColor);
    state.set_dst_alpha_blend(render_state::BlendOneMinusSrcAlpha);
    state.set_dst_blend(render_state::BlendOneMinusSrcColor);
#endif
    
    add_material("basic",
                 gpu->retain<gpu_program>(),
                 std::make_shared<render_state>(state),
                 make_uniforms_ptr({
        make_uniform("tex1", texture::null())
    }));
#endif
}

sprite_mgr::~sprite_mgr() {
    
}

sprite_material* sprite_mgr::add_material(std::string const& name,
                                          gpu_program::const_ptr && program,
                                          render_state::ptr && state,
                                          render_uniform::ptr && uniform) {
    return add_material(make_unique<sprite_material>(name, std::move(program),
                                                           std::move(state), std::move(uniform)));
}

sprite_material* sprite_mgr::find_first_material(std::string const& name) const {
    auto it = std::lower_bound(_materials.begin(), _materials.end(), name,
                               [] (spt_mat_ptr const& other, std::string const& name) {
        return other->name() < name;
    });
    if(it == _materials.end() || (*it)->name() != name){
        return nullptr;
    }else
        return it->get();
}

sprite_material* sprite_mgr::add_material(std::unique_ptr<sprite_material>&& mat) {
    auto it = std::lower_bound(_materials.begin(), _materials.end(), mat->name(),
                               [] (spt_mat_ptr const& other, std::string const& name) {
        return other->name() < name;
    });
    bool same = false;
    if(it != _materials.end() && (*it)->name() == mat->name()){
        do{
            if((*it)->compatible(*mat)){
                same = true;
                break;
            }
            ++it;
        }while(it != _materials.end() && (*it)->name() == mat->name());
    }
    
    if(same) {
        return it->get();
    } else {
        return _materials.emplace(it, std::move(mat))->get();
    }
}

template<class F>
void sprite_mgr::update_buffer(sprite2d::layout_buffer &buffer, const F &remove_or_update) {
    auto first = buffer.sprites.begin(), last = buffer.sprites.end();
    for (; first != last; ++first) {
        if (remove_or_update(*first)) {
            break;
        }
    }
    
    if(first != last) {
        for(auto current = first; ++current != last;) {
            if (!remove_or_update(*current))
                *first++ = std::move(*current);
        }
    }
    
    //buffer.need_update = buffer.need_update || first != last;
    buffer.sprites.erase(first, buffer.sprites.end());
}

void sprite_mgr::sort_sprites(const goes_t &gos) {
    // TODO: remove invisible sprites? test performance
    //auto mark = gos.front()->mark();
    
}

void sprite_mgr::update(goes_t const& gos) {
    auto transform_idx = com::transform_manager::component_idx();
    
    // step 1: update all the indices, remove deleted sprites
    for(auto& it : _buffers) {
        //bool& update = it->need_update;
        uint16_t idx = 0;

        update_buffer(*it, [&idx] (layout_buffer::sprite_t& sprite) {
            if(std::get<0>(sprite)->_mark_for_remove)
                return true;
            
#if 0
            // TODO: move the invisible sprites to the end?
            // this can also be done by re-ordering the sprites in
            // a separate function
            if(std::get<0>(sprite)->parent()->mark() != mark)
                return true;
#endif
            
            if(std::get<3>(sprite) != std::get<1>(sprite)
               || std::get<1>(sprite) != idx) {
                std::get<0>(sprite)->fill_indices(idx);
                std::get<1>(sprite) = idx;
            }
            idx += std::get<2>(sprite);
            return false;
        });
    }
    
    // step 1.1: re-order the sprites, remove the invisible sprites
    // to make it cache efficient
    sort_sprites(gos);

    // step 2: update the position if needed
    auto sprite_flag = flag_offset();
    auto transform_flag = com::transform_manager::flag_offset();
    auto combined_flag = (1 << sprite_flag) | (3 << transform_flag);
    for(auto& it : _buffers) {
        // uses the first buffer
        // TODO: asynch locking?
        auto locked = it->layout->lock_channels();
        size_t offset = 0;
        
        //bool no_read = true; // oes extend doesn't allow us to read
        for(auto each = it->sprites.begin(); each != it->sprites.end();
            ++each, locked.set_offset(offset += std::get<2>(*each))) {
            auto &sprite = *each;
            auto* spt = std::get<0>(sprite).get();
            auto dirty = (spt->parent()->flag() & combined_flag) != 0;
            auto moved = std::get<3>(sprite) != std::get<1>(sprite);

            std::get<3>(sprite) = std::get<1>(sprite);  // data synced

            if(!dirty && !moved)
                continue;
            
            auto* transform = spt->parent()->get_component<com::transform>(transform_idx);
            if(!transform) {
                ; // TODO: log
                continue;
            }
            
            spt->fill_buffer(locked, *transform);
#if 0
            // TODO: move data instead of re-building the data??
            if(!no_read)
                std::memcpy(buffer, buffer_start + std::get<3>(sprite) * stride,
                            stride * std::get<2>(sprite));
#endif
         }
        
        locked.unlock();
        //it->need_update = false;
    }
}

layout_buffer* sprite_mgr::assign_buffer(sprite* spt, uint32_t count, uint32_t typeIdx) {
    layout_buffer *buf = nullptr;
    auto it = std::lower_bound(_buffers.begin(), _buffers.end(), typeIdx,
                               [] (std::unique_ptr<layout_buffer> const & buf, uint32_t type) {
                                   return buf->type_idx < type;
                               });
    for(auto free_it = it; it != _buffers.end() && (*it)->type_idx == typeIdx; ++free_it) {
        auto& spt = (*it)->sprites.back();
        if(std::get<1>(spt) + std::get<2>(spt) + count <= _vertex_buffer_size) {
            buf = free_it->get();
            break;
        }
#if 0
        // TODO
        // stop at the first try
        // it might be not worthy it, test performance
        break;
#endif
    }
    
    if(buf == nullptr) {
        buf = _buffers.emplace(it, create_buffer(_types[typeIdx]))->get();
    }
    
    buf->type_idx = typeIdx;
    buf->sprites.emplace_back(spt,
                              buf->sprites.size() > 0 ?
                              std::get<1>(buf->sprites.back()) + std::get<2>(buf->sprites.back()) : 0,
                              count, -1U);
    //buf->need_update = true;
    return buf;
}

std::unique_ptr<layout_buffer> sprite_mgr::create_buffer(vertices_t const& layout) {
    vertex_layout::channels_t channels;
    channels.reserve(layout.size());
    
    size_t offset = 0;
    for(auto& it : layout) {
        channels.push_back({nullptr, it.type, it.count, offset, 0});
        offset += vertex_layout::type_size(it.type) * it.count;
    }
    offset = (offset + 3) & ~(0x3); // align to multiple of 4 bytes
    
    auto buffer = _device->create_buffer(_vertex_buffer_size * offset,
                                         vertex_buffer::Dynamic);
    for(auto& it : channels) {
        it.buffer = buffer->retain<vertex_buffer>();
        it.stride = offset;
    }
    
    auto vlayout = _device->create_layout(std::move(channels),
                                           _device->create_index_buffer(_index_buffer_size * sizeof(uint16_t),
                                                                        vertex_buffer::Stream),
                                           vertex_layout::Triangles);
    return make_unique<layout_buffer>(layout_buffer{std::move(vlayout), {}, 0, map_channel(layout)});
}

uint16_t sprite_mgr::add_type(vertices_t const& layout) {
    auto it = std::find(_types.begin(), _types.end(), layout);
    if(it != _types.end())
        return it - _types.begin();
    _types.push_back(layout);
    std::sort(_types.back().begin(), _types.back().end(),
              [] (sprite_vertex const& lhs, sprite_vertex const& rhs) {
                  return lhs.name < rhs.name;
              });
    
    // no identical name
    assert(std::adjacent_find(_types.back().begin(), _types.back().end(),
                              [] (sprite_vertex const& lhs, sprite_vertex const& rhs) {
                                  return lhs.name == rhs.name;
                              }) == _types.back().end());
    return _types.size() - 1;
}

std::array<int, layout_buffer::MAX> sprite_mgr::map_channel(vertices_t const& layout) const {
    std::array<int, layout_buffer::MAX> indices = {{-1,-1,-1}};
    for(int i = 0; i<_channel_names.size(); ++i) {
        auto it = std::lower_bound(layout.begin(), layout.end(), _channel_names[i],
                                   [] (sprite_vertex const& vtx, std::string const& channel) {
                                       return vtx.name < channel;
                                   });
        if(it != layout.end() && it->name == _channel_names[i]) {
            indices[i] = std::distance(layout.begin(), it);
        }
    }
    return std::move(indices);
}

std::vector<std::string> sprite_mgr::vertex_layout(uint16_t type_idx) const {
    assert(type_idx >= 0 && type_idx < _types.size());
    auto& t = _types[type_idx];
    std::vector<std::string> layout;
    layout.resize(t.size());
    std::transform(t.begin(), t.end(), layout.begin(), [] (sprite_vertex const& v) {
        return v.name;
    });
    return std::move(layout);
}
