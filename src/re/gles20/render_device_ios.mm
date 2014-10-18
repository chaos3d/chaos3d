#include "re/gles20/render_gles20.h"
#include "re/gles20/render_device.h"
#include "re/gles20/render_view_ios.h"
#include "re/gles20/gl_context_ios.h"

#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>

namespace gles20 {

struct render_device::internal {
    EAGLSharegroup* sharegroup;
};

::render_device* create_device() {
    return new render_device();
}

render_device::~render_device() {

}

render_device::render_device() :
    _internal(new internal({nullptr})) {
}
    
bool render_device::init_context() {
    // TODO: thread safe / recycle context
    if([EAGLContext currentContext] != nil)
        return true;
    
    EAGLContext* context = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES2
                                                 sharegroup: _internal->sharegroup];
    
    if(context == nil)
        return false;
    
    _internal->sharegroup = context.sharegroup;
    [EAGLContext setCurrentContext: context];
    return true;
}

render_context* render_device::create_context(render_window*) {
    EAGLContext* context = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES2
                                                 sharegroup: _internal->sharegroup];
    
    if(context == nil)
        return nullptr;
    
    _internal->sharegroup = context.sharegroup;
    
    [EAGLContext setCurrentContext: context];
    GLint texture_units = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
    return new gl_context_ios(context, texture_units);
}
    
render_window* render_device::create_window(native_window* native_parent,
                                            render_target::target_size_t const& size_,
                                            render_window::window_pos_t const& pos_,
                                            float backing_ratio) {
    return new render_view(native_parent, size_, pos_, backing_ratio);
}

render_texture* render_device::create_render_texture() {
    //TODO
    return nullptr;
}

}
