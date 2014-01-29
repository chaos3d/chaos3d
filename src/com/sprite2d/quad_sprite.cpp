#include "com/sprite2d/quad_sprite.h"
#include "sg/transform.h"

using namespace sprite2d;

void quad_sprite::fill_buffer(void* raw, com::transform const& trans) const {
    // FIXME:
    // stride, and index
    char* buffer = reinterpret_cast<char*>(raw);
    constexpr int vertice_size = sizeof(float) * 3;
    constexpr int uv_size = sizeof(float) * 2;
    int stride = vertice_size + uv_size;
    box2f const& uv = frame();

    memcpy(buffer, trans.to_global(bound().corner(box2f::BottomLeft)).data(), sizeof(float)*3);
    memcpy(buffer + vertice_size, uv.corner(box2f::TopLeft).data(), sizeof(float)*2);
    
    memcpy(buffer+=stride, trans.to_global(bound().corner(box2f::TopLeft)).data(), sizeof(float)*3);
    memcpy(buffer + vertice_size, uv.corner(box2f::BottomLeft).data(), sizeof(float)*2);
    
    memcpy(buffer+=stride, trans.to_global(bound().corner(box2f::TopRight)).data(), sizeof(float)*3);
    memcpy(buffer + vertice_size, uv.corner(box2f::BottomRight).data(), sizeof(float)*2);
    
    memcpy(buffer, trans.to_global(bound().corner(box2f::BottomRight)).data(), sizeof(float)*3);
    memcpy(buffer + vertice_size, uv.corner(box2f::TopRight).data(), sizeof(float)*2);
}
