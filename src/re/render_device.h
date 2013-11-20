#ifndef _RENDER_DEVICE_H
#define _RENDER_DEVICE_H

#include <initializer_list>
#include "common/common.h"
#include "render_types.h"
#include "vertex_buffer.h"

class vertex_array;

class render_device {
public:
    enum{
        None = 0,
        OpenGLES11,
        OpenGLES20,
        OpenGLES30,
        
        DX11,
    };
    
    // it behaves as a singleton: it either creates
    // a new one or returnes an exisiting one (type=None)
    //
    // it could also be potential to have multiple
    // devices...
    static render_device* get_device(uint8_t type = None);
    
    virtual ~render_device() {};
    
    // initialize the context for the current thread
    // each thread should have different contexts,
    // and it's client's job to avoid race conditions
    //
    // returns false if it doesn't support the current
    // device
    virtual bool init_context() { return false; };
    
    virtual texture* create_texture() = 0;
    virtual render_texture* create_render_texture() = 0;
    virtual render_window* create_window(render_target::target_size_t const&,
                                         render_window::window_pos_t const& pos = render_window::window_pos_t(0.f,0.f)) = 0;
    
    virtual vertex_array* create_vertex(std::initializer_list<vertex_buffer*> const&) = 0;
    
    // create a single multi-channel vertex buffer
    virtual vertex_buffer* create_buffer(std::initializer_list<vertex_buffer::channel_desc> const&,
                                         size_t size = 0, uint8_t* data = nullptr, // initial data
                                         bool client = false,
                                         bool interleaved = true) = 0;
};

#endif