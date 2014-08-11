#include "re/gles20/render_window_mac.h"

using namespace gles20;

@interface EGLView : NSView {
    
}

@end

@implementation EGLView

//TODO handle events

@end

render_window_mac::render_window_mac(EGLDisplay display,
                                     target_size_t const& size, window_pos_t const& pos)
: render_window_egl(size, pos) {
    create_native();
    create_surface(display);
}

void render_window_mac::create_native() {
    // TODO: retina window
    NSRect frame = NSMakeRect(get_position()(0), get_position()(1),
                              size()(0), size()(1));
    
    // Create our view
    _view = [[EGLView alloc] initWithFrame: frame];
}

void render_window_mac::create_surface(EGLDisplay display) {
    // TODO: depth buffer etc
    EGLint attrs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };
    
    EGLint num_configs;
    EGLConfig egl_config;
    
    if (!eglChooseConfig(display, attrs, &egl_config, 1, &num_configs) || (num_configs != 1)) {
        assert(false);
    }
    
    _surface = eglCreateWindowSurface(display, egl_config, (EGLNativeWindowType) _view, NULL);
    assert(_surface != EGL_NO_SURFACE);
}