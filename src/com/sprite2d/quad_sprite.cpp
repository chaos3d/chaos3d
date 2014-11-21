#include "com/sprite2d/quad_sprite.h"
#include "sg/transform.h"
#include "re/render_uniform.h"
#include "com/sprite2d/texture_atlas.h"

using namespace sprite2d;

quad_sprite::quad_sprite(game_object* go, int type)
: sprite(go, 4, type), _alpha(1.f)
{
    
}

quad_sprite::quad_sprite(game_object* go, texture_atlas const& atlas,
                         std::string const& name,
                         std::string const& mat)
: quad_sprite(go, 0) {
    set_from_atlas(atlas, name, mat);
}

quad_sprite* quad_sprite::clone(game_object* go) const {
    int type = sprite_mgr::instance().buffer_index(_data.buffer);
    quad_sprite* quad = new quad_sprite(go, type);
    quad->_data.material = _data.material;
    quad->_frame = _frame;
    quad->_bound = _bound;
    quad->_alpha = _alpha;
    quad->mark_dirty();
    
    return quad;
}

void quad_sprite::fill_indices(uint16_t start_idx) {
    _indices = {
        start_idx, static_cast<uint16_t>(start_idx + 1),
        static_cast<uint16_t>(start_idx + 2), static_cast<uint16_t>(start_idx + 1),
        static_cast<uint16_t>(start_idx + 2), static_cast<uint16_t>(start_idx + 3)
    };
}

box2f quad_sprite::get_bounding_box() const {
    box2f bb;
    for (auto& v : bound()) {
        bb.extend(v);
    }
    return bb;
}

void quad_sprite::set_bound_from_box(box2f const& box) {
    set_bound({{
        box.corner(box2f::BottomLeft),
        box.corner(box2f::TopLeft),
        box.corner(box2f::BottomRight),
        box.corner(box2f::TopRight),
    }});
}

quad_sprite& quad_sprite::set_from_material(sprite_material* mat,
                                            sprite_v_t const& frame,
                                            vector2f const& pivot) {
    if (mat)
        set_material(mat);
    
    texture *const* tex = nullptr;
    shared_material()->uniform()->get("c_tex1", tex);
    if (tex == nullptr || *tex == nullptr)
        return *this;
    
    auto& size = (*tex)->size();
    vector2f xaxis(frame[2] - frame[0]);
    vector2f yaxis(frame[1] - frame[0]);
    // assume only axis-aligned
    assert(std::abs(yaxis.x() - FLT_EPSILON) <= FLT_EPSILON || std::abs(yaxis.y() - FLT_EPSILON) <= FLT_EPSILON);
    assert(std::abs(xaxis.x() - FLT_EPSILON) <= FLT_EPSILON || std::abs(xaxis.y() - FLT_EPSILON) <= FLT_EPSILON);
    vector2f bd(std::abs(xaxis.x()) > FLT_EPSILON ?
                std::abs(xaxis.x()) * size.x() : std::abs(xaxis.y()) * size.y(),
                std::abs(yaxis.x()) > FLT_EPSILON ?
                std::abs(yaxis.x()) * size.x() : std::abs(yaxis.y()) * size.y());
    set_bound({{
        vector2f(-bd.x()/2.f, -bd.y()/2.f) - pivot,
        vector2f(-bd.x()/2.f, bd.y()/2.f) - pivot,
        vector2f(bd.x()/2.f, -bd.y()/2.f) - pivot,
        vector2f(bd.x()/2.f, bd.y()/2.f) - pivot
    }});
    set_frame(frame);
    return *this;
}

quad_sprite& quad_sprite::set_from_atlas(texture_atlas const& atlas, std::string const& name,
                                         std::string const& mat) {
    set_material(mat, { make_uniform("c_tex1", atlas.texture_ptr().get()) });
    return set_from_material(nullptr, atlas.get_frame(name));
}

void quad_sprite::fill_buffer(vertex_layout::locked_buffer const& buffer,
                              com::transform const& trans) const {
    auto& indices = _data.buffer->channel_indices;
    
    // TODO: optimize? metal? simd?
    if (indices[layout_buffer::POSITION] >= 0) {
        char* raw = buffer.buffer(indices[layout_buffer::POSITION]);
        assert(buffer.type(indices[layout_buffer::POSITION]) == vertex_layout::Float); // no conversion
        size_t data_size = std::min(buffer.unit(indices[layout_buffer::POSITION]), 4) * sizeof(float);
        size_t stride = buffer.stride(indices[layout_buffer::POSITION]);
        
        auto pos = trans.to_global(bound()[0]);
        memcpy(raw, (vector4f(pos.x(), pos.y(), pos.z(), _alpha)).data(), data_size);
        pos = std::move(trans.to_global(bound()[1]));
        memcpy(raw+=stride, (vector4f(pos.x(), pos.y(), pos.z(), _alpha)).data(), data_size);
        pos = std::move(trans.to_global(bound()[2]));
        memcpy(raw+=stride, (vector4f(pos.x(), pos.y(), pos.z(), _alpha)).data(), data_size);
        pos = std::move(trans.to_global(bound()[3]));
        memcpy(raw+ stride, (vector4f(pos.x(), pos.y(), pos.z(), _alpha)).data(), data_size);
    }

    if (indices[layout_buffer::UV] >= 0) {
        auto& uv = frame();
        char* raw = buffer.buffer(indices[layout_buffer::UV]);
        assert(buffer.type(indices[layout_buffer::UV]) == vertex_layout::Float); // no conversion
        size_t data_size = std::min(buffer.unit(indices[layout_buffer::UV]), 2) * sizeof(float);
        size_t stride = buffer.stride(indices[layout_buffer::UV]);
        memcpy(raw, uv[0].data(), data_size);
        memcpy(raw+=stride, uv[1].data(), data_size);
        memcpy(raw+=stride, uv[2].data(), data_size);
        memcpy(raw+ stride, uv[3].data(), data_size);
    }
}
