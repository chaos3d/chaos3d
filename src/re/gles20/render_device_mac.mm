#include "re/gles20/render_gles20.h"
#include "re/gles20/render_device.h"
#include "re/gles20/render_device-internal.h"
#include "re/gles20/gl_context_egl.h"
#include "re/gles20/render_window_mac.h"

#import <EGL/egl.h>

namespace gles20 {
    
    render_window* render_device::create_window(void* native_parent,
                                                render_target::target_size_t const& size_,
                                                render_window::window_pos_t const& pos_,
                                                float backing_ratio) {
        return new render_window_mac((__bridge id)native_parent, _internal->display, size_, pos_, backing_ratio);
    }
}
