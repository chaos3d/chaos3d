#ifndef _CHAOS3D_RE_GLES20_GL_RENDER_WINDOW_EGL_H
#define _CHAOS3D_RE_GLES20_GL_RENDER_WINDOW_EGL_H

#include <EGL/egl.h>
#include "gles2.h"
#include "re/render_window.h"
#include "re/gles20/gl_context_egl.h"

namespace gles20 {
    
    class render_window_egl : public render_window {
    public:
        render_window_egl(target_size_t const& size, window_pos_t const& pos,
                          float backing_ratio)
        : render_window(size, pos, backing_ratio)
        {
        }
        
        virtual ~render_window_egl();

        EGLSurface surface() const { return _surface; }
        
    protected:
        virtual void set_viewport(rect2d const& view) override;
        virtual void clear(int mask, color_t const& color = {}) override;
        virtual void clear_stencil(int set) override;
        virtual vector3f normalize_position(vector3f const& screen_pos,
                                            rect2d const& viewport) const override;
        
        virtual bool bind(render_context*) override;
        virtual bool flush(render_context*) override;
        
    protected:
        EGLSurface _surface; // subclass to initialize this
    };
    
}

#endif