#include "re/gles20/render_window_mac.h"

using namespace gles20;

@interface EGLView : NSView {
    
}

@end

@implementation EGLView

//TODO handle events

@end

render_window_mac::render_window_mac(EGLDisplay display,
                                     target_size_t const& size,
                                     window_pos_t const& pos,
                                     float backing_ratio)
: render_window_egl(size, pos, backing_ratio), _view(nil) {
    create_native();
    create_surface(display);
}

// TODO: this can be removed if the scaling factor is set manually
render_window::window_pos_t render_window_mac::convert_to_backing(window_pos_t const& pos) {
    // make sure they are the same for now
    assert(_view == nil || abs(_view.layer.contentsScale - get_backing_ratio()) <= DBL_EPSILON);
    return pos * get_backing_ratio();
}

render_window::window_pos_t render_window_mac::convert_from_backing(window_pos_t const& pos) {
    assert(_view == nil || abs(_view.layer.contentsScale - get_backing_ratio()) <= DBL_EPSILON);
    return pos / get_backing_ratio();
}

void render_window_mac::create_native() {
    target_size_t size = convert_from_backing(get_size());
    NSRect frame = NSMakeRect(get_position()(0), get_position()(1),
                              size(0), size(1));
    
    // Create our view
    _view = [[EGLView alloc] initWithFrame: frame];

    // TODO: set content scale manually
    // we only suppports 2 ratios now
    assert(get_backing_ratio() == 2.f ||
           get_backing_ratio() == 1.f);
    if (get_backing_ratio() == 2.f)
        [_view setWantsBestResolutionOpenGLSurface: YES];
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