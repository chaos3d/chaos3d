#include "re/gles20/gl_render_window_egl.h"
#include "re/gles20/gl_context_egl.h"

using namespace gles20;

render_window_egl::~render_window_egl() {
    
}

bool render_window_egl::bind(render_context* context) {
    assert(dynamic_cast<gl_context_egl*>(context) != nullptr);
    gl_context_egl* context_egl = static_cast<gl_context_egl*>(context);
    
    return context_egl->set_current(_surface);
}

bool render_window_egl::flush(render_context* context) {
    assert(dynamic_cast<gl_context_egl*>(context) != nullptr);
    gl_context_egl* context_egl = static_cast<gl_context_egl*>(context);

    return eglSwapBuffers(context_egl->display(), _surface) == EGL_TRUE;
}

void render_window_egl::clear(int mask, color_t const& color) {
    glClearColor(color(0), color(1), color(2), color(3));
    
    glClear(((mask & COLOR) ? GL_COLOR_BUFFER_BIT : 0) |
            ((mask & DEPTH) ? GL_DEPTH_BUFFER_BIT : 0) );
    
    GLNOERROR;
}

render_window_egl::vector3f render_window_egl::normalize_position(vector3f const& screen_pos,
                                                                  rect2d const& viewport) const {
    return vector3f(
                    (screen_pos.x() - viewport.min().x()) / viewport.sizes().x() * 2.f -1.f,
                    1.f - (screen_pos.y() - viewport.min().y()) / viewport.sizes().y() * 2.f,
                    screen_pos.z() * 2.f - 1.f
                    );
    
}

void render_window_egl::clear_stencil(int set) {
    glClearStencil(set);
    GLNOERROR;
}

void render_window_egl::set_viewport(rect2d const& view) {
    glViewport(view.min().x(), view.min().y(),
               view.max().x(), view.max().y());
    GLNOERROR;
}
