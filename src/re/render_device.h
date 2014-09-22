#ifndef _RENDER_DEVICE_H
#define _RENDER_DEVICE_H

#include <initializer_list>
#include "common/common.h"
#include "re/texture.h"
#include "re/vertex_buffer.h"
#include "re/render_target.h"
#include "re/render_window.h" // TODO: remove this
#include "re/gpu_program.h"
#include "re/render_context.h"
#include "re/vertex_layout.h"

class render_window;
class render_device_capacity;

class render_device {
public:
    enum{
        None = 0,
        OpenGLES11,
        OpenGLES20,
        OpenGLES30,
        
        DX11,
    };
    
    template<class T>
    class context_deleter {
    public:
        context_deleter(render_device *device) : _device(device)
        {}
        
        void operator() (T* obj) const {
            _device->release<T>(obj);
        };
        
    private:
        render_device* _device;
    };
    
public:
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
    virtual render_context* create_context(render_window*) = 0;
    virtual void release_context(render_context*);
    
    virtual texture::ptr create_texture(texture::vector2i const&, texture::attribute_t const&) = 0;
    virtual render_texture* create_render_texture() = 0;
    virtual render_window* create_window(void* native_parent,
                                         render_target::target_size_t const&,
                                         render_window::window_pos_t const& = render_window::window_pos_t(0.f,0.f),
                                         float backing_ratio = 1.f) = 0;
    virtual vertex_layout::ptr create_layout(vertex_layout::channels_t&&,
                                             vertex_index_buffer::ptr&&, uint8_t mode) = 0;

    // the device capacity
    virtual render_device_capacity const& get_capacity() const = 0;
    
    // shaders
    virtual gpu_program::ptr create_program() = 0;
    virtual gpu_shader::ptr create_shader(int type) = 0;
    
    // create a single multi-channel vertex buffer
    virtual vertex_buffer::ptr create_buffer(size_t size, int type) = 0;
    virtual vertex_index_buffer::ptr create_index_buffer(size_t size, int type) = 0;

    // helper method for GP
    template<class T> void release(T* obj);
    template<class T> context_deleter<T> deleter() { return context_deleter<T>(this); }
};

template<> void render_device::release<render_context>(render_context* obj);

#endif