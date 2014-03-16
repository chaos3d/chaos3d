#ifndef _RENDER_STATE_H
#define _RENDER_STATE_H

#include <memory>
#include <unordered_map>
#include <Eigen/Dense>
#include "common/utility.h"

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

    static render_state::ptr default_state_copy() {
        return render_state::ptr(new render_state(*default_state()));
    }
    
    render_state()
    : _depth_func(DepthNone),
    _src_blend(BlendNone), _dst_blend(BlendNone),
    _src_alpha_blend(BlendNone), _dst_alpha_blend(BlendNone),
    _blend_color(1.f,1.f,1.f,1.f), _blend_op(0), _alpha_blend_op(0),
    _culling(0)
    {
        memset(&padding, 0, sizeof(padding));
    }
    
    ATTRIBUTE(uint8_t, depth_func, 0);
    ATTRIBUTE(uint8_t, src_blend, 0);
    ATTRIBUTE(uint8_t, dst_blend, 0);
    ATTRIBUTE(uint8_t, src_alpha_blend, 0);
    ATTRIBUTE(uint8_t, dst_alpha_blend, 0);
    ATTRIBUTE(uint8_t, blend_op, 0);
    ATTRIBUTE(uint8_t, alpha_blend_op, 0);
    ATTRIBUTE(uint8_t, culling, 0);
    uint8_t padding[8];
    ATTRIBUTE(color_t, blend_color, color_t());
    
    bool operator == (render_state const& rhs) const {
        return memcmp(this, &rhs, sizeof(render_state)) == 0;
    }
    
    // viewport/scissor
};

#endif