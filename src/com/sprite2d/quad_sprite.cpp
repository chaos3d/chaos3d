#include "com/sprite2d/quad_sprite.h"
#include "sg/transform.h"

using namespace sprite2d;

quad_sprite::quad_sprite(game_object* go, int type)
: sprite(go, 4, type), _alpha(1.f)
{
    
}

quad_sprite* quad_sprite::clone(game_object* go) const {
    quad_sprite* quad = new quad_sprite(go, 0); //FIXME: type
    quad->_data.material = _data.material;
    quad->_frame = _frame;
    quad->_bound = _bound;
    quad->_alpha = _alpha;
    quad->mark_dirty();
    
    return quad;
}

void quad_sprite::fill_indices(uint16_t start_idx) {
    _indices = {
        start_idx, start_idx+1, start_idx+2,
        start_idx+1, start_idx+2, start_idx+3
    };
}

void quad_sprite::fill_buffer(vertex_layout::locked_buffer const& buffer,
                              com::transform const& trans) const {
    auto& indices = _data.buffer->channel_indices;
    
    if(indices[layout_buffer::POSITION] >= 0) {
        char* raw = buffer.buffer(indices[layout_buffer::POSITION]);
        assert(buffer.type(indices[layout_buffer::POSITION]) == vertex_layout::Float); // no conversion
        size_t data_size = std::min(buffer.unit(indices[layout_buffer::POSITION]), 4) * sizeof(float);
        size_t stride = buffer.stride(indices[layout_buffer::POSITION]);
        
        auto pos = trans.to_global(bound().corner(box2f::BottomLeft));
        memcpy(raw, (vector4f(pos.x(), pos.y(), pos.z(), _alpha)).data(), data_size);
        pos = std::move(trans.to_global(bound().corner(box2f::TopLeft)));
        memcpy(raw+=stride, (vector4f(pos.x(), pos.y(), pos.z(), _alpha)).data(), data_size);
        pos = std::move(trans.to_global(bound().corner(box2f::BottomRight)));
        memcpy(raw+=stride, (vector4f(pos.x(), pos.y(), pos.z(), _alpha)).data(), data_size);
        pos = std::move(trans.to_global(bound().corner(box2f::TopRight)));
        memcpy(raw+ stride, (vector4f(pos.x(), pos.y(), pos.z(), _alpha)).data(), data_size);
    }

    if(indices[layout_buffer::UV] >= 0) {
        box2f const& uv = frame();
        char* raw = buffer.buffer(indices[layout_buffer::UV]);
        assert(buffer.type(indices[layout_buffer::UV]) == vertex_layout::Float); // no conversion
        size_t data_size = std::min(buffer.unit(indices[layout_buffer::UV]), 2) * sizeof(float);
        size_t stride = buffer.stride(indices[layout_buffer::UV]);
        memcpy(raw, uv.corner(box2f::TopLeft).data(), data_size);
        memcpy(raw+=stride, uv.corner(box2f::BottomLeft).data(), data_size);
        memcpy(raw+=stride, uv.corner(box2f::TopRight).data(), data_size);
        memcpy(raw+ stride, uv.corner(box2f::BottomRight).data(), data_size);
    }
}
