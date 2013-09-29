#ifndef _RENDER_STATE_H
#define _RENDER_STATE_H

class render_state{
public:
    enum {
        MaxTextureUnit = 16,
    };
    
private:
    uint8_t _depth_func;
    uint8_t _depth_enabled;
    uint8_t _blending;
    uint8_t _src_blend, _dst_blend, _src_alpha_blend, _dst_alpha_blend;
    uint8_t _blend_op, _alpha_blend_op;
    uint8_t _culling;
    uint16_t _texture_units[MaxTextureUnit];
    uint8_t _active_texture_cnt;
    
    // viewport/transform/scissor/gpu_program
};
#endif