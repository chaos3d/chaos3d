#ifndef _GLES20_RENDER_DEVICE_H
#define _GLES20_RENDER_DEVICE_H

#include "re/render_device.h"
#include "re/render_target.h"
#include "re/gles20/gl_texture.h"
#include "re/gles20/gl_vertex_buffer.h"
#include "re/gles20/gl_gpu.h"

#if defined(__OBJC__)
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#endif

namespace gles20 {

class render_device : public ::render_device {
public:
    render_device();
    virtual ~render_device();
    virtual bool init_context() override;
    
    virtual gl_texture* create_texture() override;
    virtual render_texture* create_render_texture() override;
    virtual render_window* create_window(render_target::target_size_t const&,
                                         render_window::window_pos_t const&) override;

    //virtual vertex_array* create_vertex(std::initializer_list<vertex_buffer*> const&) override;
    
    virtual gl_gpu_program* create_program() override;
    virtual gl_gpu_shader* create_shader(int type) override;

    // create a single multi-channel vertex buffer
    virtual gl_vertex_buffer* create_buffer(size_t size, int type) override;
    virtual gl_vertex_index_buffer* create_index_buffer(size_t size, int type) override;

private:
#if defined(__OBJC__)
    EAGLSharegroup* _sharegroup;
#else
#endif
};

}

#endif