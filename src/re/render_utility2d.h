#ifndef _RENDER_UTILITY2D_H
#define _RENDER_UTILITY2D_H

class render_utility2d {
public:
    static void draw_quad(float width, float height, void* data);
    static void draw_quad_with_texture(float width, float height, float u, float v,
                                       float tex_w, float tex_h, void* data);
};

#endif