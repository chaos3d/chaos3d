#ifndef _RENDER_STATE_H
#define _RENDER_STATE_H

#include "common/utility.h"
#include <unordered_map>
#include <Eigen/Dense>
#include <memory>

// fixed pipeline state
class render_state{
public:
    typedef Eigen::Vector4f color_t;
    
    // depth function
    enum {
        DepthNone,
        DepthNever, DepthAlways, DepthLess, DepthLequal,
        DepthEqual, DepthNotequal, DepthGequal, DepthGreater
    };

    // blend function
    enum {
        BlendNone,
        BlendOne, BlendZero, BlendDstColor, BlendSrcColor,
        BlendOneMinusDstColor, BlendOneMinusSrcColor,
        BlendDstAlpha, BlendSrcAlpha,
        BlendOneMinusDstAlpha, BlendOneMinusSrcAlpha
    };
    
    typedef std::shared_ptr<render_state> ptr;
    typedef std::shared_ptr<render_state const> const_ptr;

public:
    static render_state::const_ptr default_state() {
        static render_state::const_ptr state(new render_state());
        return state;
    }

    ATTRIBUTE(uint8_t, depth_func);
    ATTRIBUTE(uint8_t, blending);
    ATTRIBUTE(uint8_t, src_blend);
    ATTRIBUTE(uint8_t, dst_blend);
    ATTRIBUTE(uint8_t, src_alpha_blend);
    ATTRIBUTE(uint8_t, dst_alpha_blend);
    ATTRIBUTE(uint8_t, blend_op);
    ATTRIBUTE(uint8_t, alpha_blend_op);
    ATTRIBUTE(uint8_t, culling);
    ATTRIBUTE(color_t, blend_color);
    
    bool operator == (render_state const rhs) const {
        return memcmp(this, &rhs, sizeof(render_state)) == 0;
    }
    
    // viewport/scissor
};

#endif