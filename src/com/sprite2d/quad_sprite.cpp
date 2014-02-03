#include "com/sprite2d/quad_sprite.h"
#include "sg/transform.h"

using namespace sprite2d;

quad_sprite::quad_sprite(game_object* go, int type)
: sprite(go, 4, type)
{
    _indices = {0,1,2,1,2,3};
}

quad_sprite* quad_sprite::clone(game_object* go) const {
    quad_sprite* quad = new quad_sprite(go, 0); //FIXME: type
    quad->_data.material = _data.material;
    quad->_frame = _frame;
    quad->_bound = _bound;
    quad->mark_dirty();
    
    return quad;
}


void quad_sprite::fill_buffer(void* raw, size_t stride, com::transform const& trans) const {
    // FIXME:
    // stride, and index
    char* buffer = reinterpret_cast<char*>(raw);
    constexpr int vertice_size = sizeof(float) * 4;
    constexpr int uv_size = sizeof(float) * 2;
    box2f const& uv = frame();
    float alpha = .5f;

    memcpy(buffer, trans.to_global(bound().corner(box2f::BottomLeft)).data(), sizeof(float) * 3);
    memcpy(buffer + vertice_size, uv.corner(box2f::TopLeft).data(), uv_size);
    memcpy(buffer + sizeof(float) * 3, &alpha, sizeof(float));
    
    memcpy(buffer+=stride, trans.to_global(bound().corner(box2f::TopLeft)).data(), sizeof(float) * 3);
    memcpy(buffer + vertice_size, uv.corner(box2f::BottomLeft).data(), uv_size);
    memcpy(buffer + sizeof(float) * 3, &alpha, sizeof(float));
    
    memcpy(buffer+=stride, trans.to_global(bound().corner(box2f::BottomRight)).data(), sizeof(float) * 3);
    memcpy(buffer + vertice_size, uv.corner(box2f::TopRight).data(), uv_size);
    memcpy(buffer + sizeof(float) * 3, &alpha, sizeof(float));

    memcpy(buffer+=stride, trans.to_global(bound().corner(box2f::TopRight)).data(), sizeof(float) * 3);
    memcpy(buffer + vertice_size, uv.corner(box2f::BottomRight).data(), uv_size);
    memcpy(buffer + sizeof(float) * 3, &alpha, sizeof(float));
}
