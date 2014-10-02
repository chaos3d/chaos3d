#include "re/gles20/render_window_mac.h"

using namespace gles20;

@interface EGLView : NSView {
    
}

@end

@implementation EGLView

//TODO handle events

@end

render_window_mac::render_window_mac(id parent,
                                     EGLDisplay display,
                                     target_size_t const& size,
                                     window_pos_t const& pos,
                                     float backing_ratio)
: render_window_egl(parent, size, pos, backing_ratio), _view(nil) {
    create_native(parent);
    create_surface(display, _view);
}

render_window_mac::~render_window_mac() {
    [_view removeFromSuperview];
    [_view release];
    _view = nil;
}

// TODO: this can be removed if the scaling factor is set manually
render_window::window_pos_t render_window_mac::convert_to_backing(window_pos_t const& pos) const {
    // make sure they are the same for now
    assert(_view == nil || abs(_view.layer.contentsScale - get_backing_ratio()) <= DBL_EPSILON);
    return render_window::convert_to_backing(pos);
}

render_window::window_pos_t render_window_mac::convert_from_backing(window_pos_t const& pos) const {
    assert(_view == nil || abs(_view.layer.contentsScale - get_backing_ratio()) <= DBL_EPSILON);
    return render_window::convert_from_backing(pos);
}

void render_window_mac::create_native(id parent) {
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
    
    if ([parent isKindOfClass: [NSWindow class]]) {
        [((NSWindow*) parent) setContentView: _view];
    } else if ([parent isKindOfClass: [NSView class]]) {
        [((NSView*) parent) addSubview: _view];
    }
}
