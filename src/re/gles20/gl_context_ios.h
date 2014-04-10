#ifndef _CHAOS3D_RE_GLES20_GL_CONTEXT_IOS_H
#define _CHAOS3D_RE_GLES20_GL_CONTEXT_IOS_H

#include <OpenGLES/EAGL.h>
#include "re/gles20/gl_context.h"

class gl_context_ios : public gl_context {
public:
    gl_context_ios(EAGLContext* context, size_t max)
    : _context(context), gl_context(max)
    {
    }
    
    virtual void set_current() override {
        [EAGLContext setCurrentContext: context()];
    }

    EAGLContext* context() const { return _context; }
    
private:
    EAGLContext* _context;
};

#endif