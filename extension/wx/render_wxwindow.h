#ifndef _WX_GLES20_RENDER_WXWINDOW_H
#define _WX_GLES20_RENDER_WXWINDOW_H

#include "re/gles20/gl_render_window_egl.h"

class wxWindow;
class wxGameWindow;

namespace gles20 {
    class render_device;
    
    class render_wxwindow_egl : public render_window_egl {
    public:
        render_wxwindow_egl(wxWindow* parent, EGLDisplay display,
                            target_size_t const& size, window_pos_t const& pos);
        virtual ~render_wxwindow_egl();
        
        virtual void *native_handle() override { return _window; }
        
        void invalidate() { _window = nullptr; }
        
    public:
        static render_wxwindow_egl* create(render_device*,
                                           wxWindow* parent,
                                           render_target::target_size_t const&,
                                           render_window::window_pos_t const&
                                           );
    private:
        wxGameWindow* _window;  // weak pointer, due to how wxwidgets manage
    };
}

#endif
