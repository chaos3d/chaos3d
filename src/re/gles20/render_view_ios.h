#if !defined(_RENDER_VIEW_IOS_H) && defined(__OBJC__)
#define _RENDER_VIEW_IOS_H

#include "render_target.h"

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#import <OpenGLES/EAGL.h>

@class EAGLView;

namespace gles20 {
    
class render_view : public render_window, protected render_frame{
public:
    render_view(target_size_t const&, window_pos_t const& pos = window_pos_t(0.f,0.f));
    virtual ~render_view();
    
    virtual void *native_handle() { return _native_view; };
    
protected:
    virtual bool bind();
    virtual bool flush();
    void create_native();
    void create_view();
    
private:
    EAGLContext* _context;
    EAGLView* _native_view;
    GLuint _framebuffer_id, _colorbuffer_id;
    GLuint _depthbuffer_id, _stencil_id;
};

}

#endif