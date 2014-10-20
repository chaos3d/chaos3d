#ifndef _CHAOS3D_RE_GLES20_GL_CONTEXT_EGL_H
#define _CHAOS3D_RE_GLES20_GL_CONTEXT_EGL_H

#include "re/gles20/gl_context.h"
#import <EGL/egl.h>

class gl_context_egl : public gl_context {
public:
    gl_context_egl(EGLDisplay display, EGLSurface surface,
                   EGLContext context,  size_t max)
    : gl_context(max),
    _display(display), _context(context), _surface(surface)
    {}
    
    EGLContext context() const { return _context; }
    EGLDisplay display() const { return _display; }
    
    // bind the current context with the given surface
    bool set_current(EGLSurface surface) {
        return eglMakeCurrent(_display, surface, surface, _context) == EGL_TRUE;
    }
    
    // bind the current context with the default surface (usually the main window)
    virtual render_context& set_current() override {
        eglMakeCurrent(_display, _surface, _surface, _context);
        assert(eglGetError() == EGL_SUCCESS);
        return *this;
    }
    
private:
    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext _context;
};

#endif