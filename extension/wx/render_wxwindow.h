#ifndef _WX_GLES20_RENDER_WXWINDOW_H
#define _WX_GLES20_RENDER_WXWINDOW_H

#include "re/gles20/gl_render_window_egl.h"

class wxGameWindow;

namespace gles20 {
    class render_wxwindow_egl : public render_window_egl {
    public:
        render_wxwindow_egl(wxGameWindow* parent, EGLDisplay display,
                            target_size_t const& size, window_pos_t const& pos);
        virtual ~render_wxwindow_egl();
        
        virtual void *native_handle() override { return _window; }
        
        void invalidate() { _window = nullptr; }
    private:
        wxGameWindow* _window;  // weak pointer, due to how wxwidgets manage
    };
}

#endif
