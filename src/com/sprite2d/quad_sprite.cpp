#include "com/sprite2d/quad_sprite.h"
#include "sg/transform.h"

using namespace sprite2d;

quad_sprite::quad_sprite(game_object* go, int type)
: sprite(go, 4, type)
{
}

quad_sprite* quad_sprite::clone(game_object* go) const {
    quad_sprite* quad = new quad_sprite(go, 0); //FIXME: type
    quad->_data.material = _data.material;
    quad->_frame = _frame;
    quad->_bound = _bound;
    quad->set_dirty();
    
    return quad;
}


void quad_sprite::fill_buffer(void* raw, size_t stride, com::transform const& trans) const {
    // FIXME:
    // stride, and index
    char* buffer = reinterpret_cast<char*>(raw);
    constexpr int vertice_size = sizeof(float) * 3;
    constexpr int uv_size = sizeof(float) * 2;
    box2f const& uv = frame();

    memcpy(buffer, trans.to_global(bound().corner(box2f::BottomLeft)).data(), vertice_size);
    memcpy(buffer + vertice_size, uv.corner(box2f::TopLeft).data(), uv_size);
    
    memcpy(buffer+=stride, trans.to_global(bound().corner(box2f::TopLeft)).data(), vertice_size);
    memcpy(buffer + vertice_size, uv.corner(box2f::BottomLeft).data(), uv_size);
    
    memcpy(buffer+=stride, trans.to_global(bound().corner(box2f::TopRight)).data(), vertice_size);
    memcpy(buffer + vertice_size, uv.corner(box2f::BottomRight).data(), uv_size);
    
    memcpy(buffer, trans.to_global(bound().corner(box2f::BottomRight)).data(), vertice_size);
    memcpy(buffer + vertice_size, uv.corner(box2f::TopRight).data(), uv_size);
}
