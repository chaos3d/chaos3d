#include "re/gles20/render_gles20.h"
#include "re/gles20/render_device.h"
#include "re/gles20/gl_context_egl.h"
#include "re/gles20/gl_render_window_egl.h"

#import <EGL/egl.h>

namespace gles20 {
    
    struct render_device::internal {
        EGLDisplay display = EGL_NO_DISPLAY;
        EGLContext context = EGL_NO_CONTEXT;
    };
    
    ::render_device* create_device() {
        return new render_device();
    }
    
    render_device::~render_device() {
        
    }
    
    render_device::render_device() :
    _internal(new internal()) {
    }
    
    bool render_device::init_context() {
        // TODO: thread safe / recycle context
        _internal->display = eglGetDisplay((EGLNativeDisplayType) 0);

        EGLint major_ver, minor_ver;
        if (!eglInitialize(_internal->display, &major_ver, &minor_ver)) {
            return false;
        }
        
        // TODO: log version
        
        if (!eglBindAPI(EGL_OPENGL_ES_BIT)) {
            return false;
        }

        if (!eglSwapInterval(_internal->display, 1)) {
            return false;
        }
            
        return true;
    }
    
    render_context* render_device::create_context(render_window* window) {
        EGLint attrs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
        };
        
        EGLint num_configs;
        EGLConfig egl_config;
        
        if (!eglChooseConfig(_internal->display, attrs, &egl_config, 1, &num_configs)
            || (num_configs != 1)) {
            return nullptr;
        }
        
        bool is_created = _internal->context != EGL_NO_CONTEXT;
        EGLContext context = eglCreateContext(_internal->display, egl_config, _internal->context, NULL);
        
        if (!is_created) {
            _internal->context = context;
        }
        
        render_window_egl* egl_window = dynamic_cast<render_window_egl*>(window);
        assert(egl_window != nullptr);
        
        GLint texture_units = 0;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
        return new gl_context_egl(_internal->display, egl_window->surface(),
                                  context, texture_units);
    }
    
    render_window* render_device::create_window(render_target::target_size_t const& size_,
                                                render_window::window_pos_t const& pos_) {
        return nullptr;
    }
    
    render_texture* render_device::create_render_texture() {
        //TODO
        return nullptr;
    }
    
}
