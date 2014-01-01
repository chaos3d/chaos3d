#include "re/gles20/gl_context.h"
#include "re/gles20/gl_gpu.h"

gl_context::gl_context(EAGLContext* context, size_t max)
: _context(context), render_context(max)
{
    
}

void gl_context::set_current() {
    [EAGLContext setCurrentContext: context()];
}

#pragma mark - move up
bool gl_context::set_state(render_state const&) {
    return true;
}

bool gl_context::set_program(gpu_program const& program) {
    assert(typeid(program) == typeid(gl_gpu_program));
    return true;
}
