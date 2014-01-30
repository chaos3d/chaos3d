#ifndef _GLES20_RENDER_DEVICE_H
#define _GLES20_RENDER_DEVICE_H

#include "re/render_device.h"
#include "re/render_target.h"
#include "re/render_context.h"
#include "re/gles20/gl_texture.h"
#include "re/gles20/gl_vertex_buffer.h"
#include "re/gles20/gl_vertex_layout.h"
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
    virtual render_context* create_context() override; // TODO: thread-safe
    
    virtual gl_texture* create_texture(texture::vector2i const&, texture::attribute_t const&) override;
    virtual render_texture* create_render_texture() override;
    virtual render_window* create_window(render_target::target_size_t const&,
                                         render_window::window_pos_t const&) override;

    //virtual vertex_array* create_vertex(std::initializer_list<vertex_buffer*> const&) override;
    
    virtual gl_gpu_program* create_program() override;
    virtual gl_gpu_shader* create_shader(int type) override;

    // create a single multi-channel vertex buffer
    virtual vertex_buffer::ptr create_buffer(size_t size, int type) override;
    virtual vertex_index_buffer::ptr create_index_buffer(size_t size, int type) override;
    virtual vertex_layout::ptr create_layout(vertex_layout::channels_t&&,
                                             vertex_index_buffer::ptr&&, uint8_t mode) override;
    

private:
#if defined(__OBJC__)
    EAGLSharegroup* _sharegroup;
#else
#endif
};

}

#endif