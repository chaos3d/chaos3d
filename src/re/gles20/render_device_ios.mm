#include "render_gles20.h"
#include "render_device.h"
#include "render_view_ios.h"

#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>

namespace gles20 {

::render_device* create_device() {
    return new render_device();
}

render_device::~render_device() {

}

render_device::render_device() :
    _sharegroup(nil) {
}
    
bool render_device::init_context() {
    // TODO: thread safe / recycle context
    if([EAGLContext currentContext] != nil)
        return true;
    
    EAGLContext* context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup: _sharegroup];
    
    if(context == nil)
        return false;
    
    _sharegroup = context.sharegroup;
    [EAGLContext setCurrentContext: context];
    return true;
}

render_window* render_device::create_window(render_target::target_size_t const& size_,
                                            render_window::window_pos_t const& pos_) {
    return new render_view(size_, pos_);
}

texture* render_device::create_texture() {
    //TODO
    return nullptr;
}

render_texture* render_device::create_render_texture() {
    //TODO
    return nullptr;
}

vertex_array* render_device::create_vertex() {
    //TODO
    return nullptr;
}

}
