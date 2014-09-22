#include <wx/wxprec.h>
#include <wx/wx.h>

#include "gamewindow.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>

class MyWindow : public wxWindow
{
public:
    MyWindow()
    : m_spinTimer(this, wxID_HIGHEST + 1)
    {
        SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    }
    
    void init_gles2() {
        printf("1 \n");
        m_Display = eglGetDisplay((EGLNativeDisplayType)0);
        
        printf("2 \n");
        EGLint eglMajorVersion, eglMinorVersion;
        eglInitialize(m_Display, &eglMajorVersion, &eglMinorVersion);
        TestError();
        
        EGLConfig config;
        const EGLint configurationAttributes[] =
        {
            EGL_SURFACE_TYPE,       EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE,    EGL_OPENGL_ES2_BIT,
            EGL_NONE
        };
        
        EGLint configsReturned;
        eglChooseConfig(m_Display, configurationAttributes, &config, 1, &configsReturned);
        TestError();
        printf("3 \n");
        
        m_Surface = eglCreateWindowSurface(m_Display, config, (EGLNativeWindowType)GetHandle(), NULL);
        TestError();
        printf("4 \n");
        
        
        EGLint contextAttributes[] =
        {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };
        
        // Create the context with the context attributes supplied
        m_Context = eglCreateContext(m_Display, config, NULL, contextAttributes);
        TestError();
        printf("5 \n");
        eglBindAPI(EGL_OPENGL_ES_API);
        printf("6 %p %p %p %p\n", m_Display, m_Surface, m_Context, GetHandle());
        TestError();
        
        m_spinTimer.Start(33);
    }
    
    void OnPaint(wxPaintEvent& WXUNUSED(event)) {
        wxPaintDC dc(this);
        
        eglMakeCurrent(m_Display, m_Surface, m_Surface, m_Context);
        const wxSize ClientSize = GetClientSize();
        
        glViewport(0, 0, ClientSize.x, ClientSize.y);
        glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        eglSwapBuffers(m_Display, m_Surface);
        printf("drawing\n");
    }
    
    void TestError() {
        EGLint lastError = eglGetError();
        if (lastError != EGL_SUCCESS)
            printf("egl error: %x \n", lastError);
    }
    
    void OnSpinTimer(wxTimerEvent& event) {
        Refresh(false);
#if 0
        eglMakeCurrent(m_Display, m_Surface, m_Surface, m_Context);
        TestError();
        const wxSize ClientSize = GetClientSize();
        
        glViewport(0, 0, ClientSize.x, ClientSize.y);
        glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        //printf("drawing %d %d\n", ClientSize.x, ClientSize.y);
        eglSwapBuffers(m_Display, m_Surface);
#endif
        printf("timer: %d\n", event.GetInterval());
        
    }
private:
    EGLDisplay       m_Display;
    EGLSurface       m_Surface;
    EGLContext       m_Context;
    
    wxTimer m_spinTimer;
    
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MyWindow, wxWindow)
EVT_PAINT(MyWindow::OnPaint)
EVT_TIMER(wxID_HIGHEST + 1, MyWindow::OnSpinTimer)
wxEND_EVENT_TABLE()
