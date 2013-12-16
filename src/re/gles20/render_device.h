#ifndef _GLES20_RENDER_DEVICE_H
#define _GLES20_RENDER_DEVICE_H

#include "re/render_device.h"
#include "re/render_target.h"
#include "re/gles20/gl_texture.h"

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

    virtual vertex_array* create_vertex(std::initializer_list<vertex_buffer*> const&) override;
    
    // create a single multi-channel vertex buffer
    virtual vertex_buffer* create_buffer(std::initializer_list<vertex_buffer::channel_desc> const&,
                                         size_t size = 0, uint8_t* data = nullptr, // initial data
                                         bool client = false,
                                         bool interleaved = true) override;

private:
#if defined(__OBJC__)
    EAGLSharegroup* _sharegroup;
#else
#endif
};

}

#endif