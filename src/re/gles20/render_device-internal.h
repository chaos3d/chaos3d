#include <EGL/egl.h>

namespace gles20 {
    
    struct render_device::internal {
        EGLDisplay display = EGL_NO_DISPLAY;
        EGLContext context = EGL_NO_CONTEXT;
    };
    
}