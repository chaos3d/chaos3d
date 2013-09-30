#ifndef _GLES20_RENDER_DEVICE_H
#define _GLES20_RENDER_DEVICE_H

#include "../render_device.h"

#if defined(__OBJC__)
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#endif

namespace gles20 {

class render_device : public ::render_device {
public:
    render_device();
    virtual ~render_device();
    virtual bool init_context();
    
    virtual texture* create_texture();
    virtual render_texture* create_render_texture();
    virtual render_window* create_window(render_target::target_size_t const&,
                                         render_window::window_pos_t const&);
    virtual vertex_array* create_vertex();
    
private:
#if defined(__OBJC__)
    EAGLSharegroup* _sharegroup;
#else
#endif
};

}

#endif