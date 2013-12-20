#ifndef _RENDER_STATE_H
#define _RENDER_STATE_H

#include <unordered_map>

// fixed pipeline state
class render_state{
public:
private:
    uint8_t _depth_func;
    uint8_t _depth_enabled;
    uint8_t _blending;
    uint8_t _src_blend, _dst_blend, _src_alpha_blend, _dst_alpha_blend;
    uint8_t _blend_op, _alpha_blend_op;
    uint8_t _culling;
    uint8_t _active_texture_cnt;
    
    // viewport/scissor
};

// programmable pipeline parameters (uniforms)
//  parented searching path such that each parameter
//  will have a 'scope'
class render_uniform {
public:
    void apply();

    void add_vector(char const*);
    void add_matrix(char const*);
    void add_texture(char const*);
    
private:
    render_uniform* _parent;
};
#endif