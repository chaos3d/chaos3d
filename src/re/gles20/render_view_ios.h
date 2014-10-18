#if !defined(_RENDER_VIEW_IOS_H) && defined(__OBJC__)
#define _RENDER_VIEW_IOS_H

#include "re/render_window.h"
#include "re/gles20/gles2.h"
#include <OpenGLES/EAGL.h>

@class EAGLView;
@class UIView;

namespace gles20 {
    
class render_view : public render_window{
public:
    render_view(native_window* parent,
                target_size_t const&,
                window_pos_t const& pos = window_pos_t(0.f,0.f),
                float backing_ratio = 1.f);
    virtual ~render_view();
    
    virtual void *native_handle() { return (__bridge void*)_native_view; };
    
    using render_window::dispatch;
    
protected:
    virtual void set_viewport(rect2d const& view) override;
    virtual void clear(int mask, color_t const& color = {}) override;
    virtual void clear_stencil(int set) override;
    virtual vector3f normalize_position(vector3f const& screen_pos,
                                        rect2d const& viewport) const override;

    virtual bool bind(render_context*) override;
    virtual bool flush(render_context*) override;
    void create_native(UIView* parent);
    void create_view();
    
private:
    EAGLView* _native_view;
    GLuint _framebuffer_id, _colorbuffer_id;
    GLuint _depthbuffer_id, _stencil_id;
};

}

#endif