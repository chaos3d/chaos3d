#ifndef _CHAOS3D_RE_GLES20_RENDER_WINDOW_MAC_H
#define _CHAOS3D_RE_GLES20_RENDER_WINDOW_MAC_H

#include "re/gles20/gl_render_window_egl.h"
#include <AppKit/NSWindow.h>
#include <AppKit/NSScreen.h>
#include <AppKit/NSView.h>

namespace gles20 {
    class render_window_mac : public render_window_egl {
    public:
        render_window_mac(EGLDisplay, target_size_t const&, window_pos_t const&);
        virtual void *native_handle() override { return _window; }
        
    private:
        void create_native();
        void create_surface(EGLDisplay);
        
    private:
        NSWindow* _window;
    };
}
#endif
