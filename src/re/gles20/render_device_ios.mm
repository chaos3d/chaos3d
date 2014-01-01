#include "re/gles20/render_gles20.h"
#include "re/gles20/render_device.h"
#include "re/gles20/render_view_ios.h"
#include "re/gles20/gl_context.h"

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

render_context* render_device::create_context() {
    EAGLContext* context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup: _sharegroup];
    
    if(context == nil)
        return nullptr;
    
    _sharegroup = context.sharegroup;
    
    [EAGLContext setCurrentContext: context];
    GLint texture_units = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
    return new gl_context(context, texture_units);
}
    
render_window* render_device::create_window(render_target::target_size_t const& size_,
                                            render_window::window_pos_t const& pos_) {
    return new render_view(size_, pos_);
}

render_texture* render_device::create_render_texture() {
    //TODO
    return nullptr;
}

}
