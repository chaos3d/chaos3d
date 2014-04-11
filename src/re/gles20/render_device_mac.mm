#include "re/gles20/render_gles20.h"
#include "re/gles20/render_device.h"
#include "re/gles20/gl_context_egl.h"
#include "re/gles20/render_window_mac.h"

#import <EGL/egl.h>

namespace gles20 {
    
    // Note: copy from render_device_egl.cpp
    struct render_device::internal {
        EGLDisplay display;
        EGLContext context;
    };
    
    render_window* render_device::create_window(render_target::target_size_t const& size_,
                                                render_window::window_pos_t const& pos_) {
        return new render_window_mac(_internal->display, size_, pos_);
    }
}
