#ifndef _CHAOS3D_RE_GLES20_RENDER_WINDOW_MAC_H
#define _CHAOS3D_RE_GLES20_RENDER_WINDOW_MAC_H

#include "re/gles20/gl_render_window_egl.h"
#include <AppKit/AppKit.h>

@class EGLView;

namespace gles20 {
    class render_window_mac : public render_window_egl {
    public:
        render_window_mac(id parent, EGLDisplay, target_size_t const&,
                          window_pos_t const&, float);
        virtual ~render_window_mac();
        virtual void *native_handle() override { return (__bridge void*)_view; }

    protected:
        virtual window_pos_t convert_to_backing(window_pos_t const&) const override;
        virtual window_pos_t convert_from_backing(window_pos_t const&) const override;
        
    private:
        void create_native(id);
        
    private:
        EGLView* _view;
    };
}
#endif
