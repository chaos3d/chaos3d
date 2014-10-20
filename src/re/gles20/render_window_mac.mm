#include "re/gles20/render_window_mac.h"
#include "re/native_window.h"
#include "common/log.h"

using namespace gles20;

INHERIT_LOGGER(render_window_mac, ::render_device);

@interface EGLView : NSView {
    
}

@end

@implementation EGLView

//TODO handle events

@end

render_window_mac::render_window_mac(native_window* parent,
                                     EGLDisplay display,
                                     target_size_t const& size,
                                     window_pos_t const& pos,
                                     float backing_ratio)
: render_window_egl(parent, size, pos), _view(nil) {
    create_native((__bridge id)parent->handle(), backing_ratio);
    create_surface(display, (__bridge_retained EGLNativeWindowType)_view);
}

render_window_mac::~render_window_mac() {
    [_view removeFromSuperview];
    CFRelease((CFTypeRef)_view);
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

void render_window_mac::create_native(id parent, float backing_ratio) {
    target_size_t size = get_size();
    NSRect frame = NSMakeRect(get_position()(0), get_position()(1),
                              size(0), size(1));
    
    // Create our view
    _view = [[EGLView alloc] initWithFrame: frame];

    // TODO: move this up to a function
    float supported = [NSScreen mainScreen].backingScaleFactor;
    // only assume it's the same, or fails
    if (abs(backing_ratio - supported) <= FLT_EPSILON) {
        LOG_INFO("the screen supports the requested scaling factor:" << backing_ratio);
        [_view setWantsBestResolutionOpenGLSurface: YES];
        _backing_ratio = backing_ratio;
        set_size(size * backing_ratio);
    } else if (backing_ratio > 1.f) {
        LOG_INFO("the screen doesn't support ratio: " << backing_ratio << ", ignored");
    }
    
    if ([parent isKindOfClass: [NSWindow class]]) {
        [((NSWindow*) parent) setContentView: _view];
    } else if ([parent isKindOfClass: [NSView class]]) {
        [((NSView*) parent) addSubview: _view];
    }
}
