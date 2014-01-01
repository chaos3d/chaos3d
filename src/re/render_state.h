#ifndef _RENDER_STATE_H
#define _RENDER_STATE_H

#include "common/utility.h"
#include <unordered_map>

// fixed pipeline state
class render_state{
public:
    ATTRIBUTE(uint8_t, depth_func);
    ATTRIBUTE(uint8_t, blending);
    ATTRIBUTE(uint8_t, src_blend);
    ATTRIBUTE(uint8_t, dst_blend);
    ATTRIBUTE(uint8_t, src_alpha_blend);
    ATTRIBUTE(uint8_t, dst_alpha_blend);
    ATTRIBUTE(uint8_t, blend_op);
    ATTRIBUTE(uint8_t, alpha_blend_op);
    ATTRIBUTE(uint8_t, culling);
    
    // viewport/scissor
};

#endif