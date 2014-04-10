#ifndef _CHAOS3D_RE_GLES20_GL_CONTEXT_H
#define _CHAOS3D_RE_GLES20_GL_CONTEXT_H

#include "re/render_context.h"

class gl_context : public render_context {
public:
    gl_context(size_t max)
    : render_context(max)
    {}
    
    virtual bool set_state(render_state const&) override;
    virtual bool set_program(gpu_program const&) override;
    virtual void apply() override;
};

#endif