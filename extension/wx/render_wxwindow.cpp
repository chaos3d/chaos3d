#include <wx/wxprec.h>
#include <wx/wx.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "render_wxwindow.h"

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
    EGLDisplay       m_Display;
    EGLSurface       m_Surface;
    EGLContext       m_Context;
    
private:
    render_wxwindow_egl* _peer;
    
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxGameWindow, wxWindow)
EVT_WINDOW_DESTROY(wxGameWindow::OnDestroy)
EVT_SIZE(wxGameWindow::OnSize)
EVT_SIZING(wxGameWindow::OnSize)
wxEND_EVENT_TABLE()

// FIXME: ratio
render_wxwindow_egl::render_wxwindow_egl(wxGameWindow* parent, EGLDisplay display,
                                         target_size_t const& size, window_pos_t const& pos)
: render_window_egl(parent, size, pos, 1.f) {
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