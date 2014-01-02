#ifndef _GL_CONTEXT_H
#define _GL_CONTEXT_H

#import <OpenGLES/EAGL.h>
#include "re/render_context.h"

class gl_context : public render_context {
public:
    gl_context(EAGLContext*, size_t max);
    
    virtual bool set_state(render_state const&) override;
    virtual bool set_program(gpu_program const&) override;
    virtual void set_current() override;
    virtual void apply() override;
    
    EAGLContext* context() const { return _context; }
    
private:
    EAGLContext* _context;
};

#endif