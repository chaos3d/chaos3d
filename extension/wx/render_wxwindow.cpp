#include <wx/wxprec.h>
#include <wx/wx.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "render_wxwindow.h"
#include "re/gles20/render_device.h"
#include "re/gles20/render_device-internal.h"
#include "re/native_window.h"

using namespace gles20;

class wxGameWindow : public wxWindow
{
public:
    wxGameWindow(render_wxwindow_egl* peer)
    : _peer(peer)
    {
        SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    }
    
    void Invalidate() {
        _peer = nullptr;
    }
    
private:
    void OnSize(wxSizeEvent& evt) {
        if (_peer) {
            _peer->set_size({
                evt.GetSize().GetWidth(), evt.GetSize().GetWidth()
            });
        }
    }
    
    void OnSizing(wxSizeEvent& evt) {
    }
    
    void OnDestroy(wxWindowDestroyEvent&) {
        if (_peer) {
            _peer->invalidate();
            _peer = nullptr;
        }
    }
    
private:
    render_wxwindow_egl* _peer;
    
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxGameWindow, wxWindow)
EVT_WINDOW_DESTROY(wxGameWindow::OnDestroy)
EVT_SIZE(wxGameWindow::OnSize)
EVT_SIZING(wxGameWindow::OnSize)
wxEND_EVENT_TABLE()

class native_wx_window : public native_window {
    
};

// FIXME: ratio
render_wxwindow_egl::render_wxwindow_egl(native_window* parent, EGLDisplay display,
                                         target_size_t const& size, window_pos_t const& pos)
: render_window_egl(parent, size, pos) {
    _window = new wxGameWindow(this);
    create_surface(display, _window->GetHandle());
}

render_wxwindow_egl::~render_wxwindow_egl() {
    if (_window) {
        _window->Invalidate();
        // destroy it if it is a dangling window
        if (_window->GetParent() == nullptr) {
            _window->Destroy();
        }
        _window = nullptr;
    }
}

render_wxwindow_egl* render_wxwindow_egl::create(render_device* device,
                                                 native_window* parent,
                                                 render_target::target_size_t const& size,
                                                 render_window::window_pos_t const& pos
                                                 ){
    return new render_wxwindow_egl(parent, device->internal_()->display,
                                   size, pos);
}
