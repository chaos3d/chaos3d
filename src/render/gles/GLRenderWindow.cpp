#include "2d/gles/GLRenderWindow.h"
#include "app/app.h"
#include <windowsx.h>
#include <algorithm>

//using namespace chaos;
#define	WNDCLASS_NAME	("cWnd_iPhone")

GLRenderWindow::GLRenderWindow(){
	create();
	initContext();
};

static LRESULT CALLBACK  _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	static bool touch = false;
	Application* app = Application::getInstance();

	if (uMsg == WM_CREATE){
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)(((LPCREATESTRUCT)lParam)->lpCreateParams));
		return 0;
	}

	// look up window instance
	// note: it is possible to get a WM_SIZE before WM_CREATE
	GLRenderWindow* win = (GLRenderWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch(uMsg)
	{
	case WM_ACTIVATE:
		break;
	case WM_MOVE:
	{
	}
		break;
	case WM_DISPLAYCHANGE:
	{
	}
		break;
	case WM_SIZE:
	{
	}
		break;
	case WM_LBUTTONDOWN:
	{
		Interaction *it = app->getInteraction();
		if( it != 0 ){
			touch = true;
			win->getTouches().update(0) = TouchEvent::Touch(
				app->global(),1,TouchEvent::TouchBegan, 
				Vector2f((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam)));
			it->onTouchBegan(win->getTouches(), win->getTouches().update(0));

			win->getTouches().update(0).phase = TouchEvent::TouchNormal;
		}
	}
		break;
	case WM_LBUTTONUP:
	{
		Interaction *it = app->getInteraction();
		if( touch && it != 0 ){
			TouchEvent::Touch &touch = win->getTouches().update(0);
			touch.phase = TouchEvent::TouchEnded;
			touch.lastPos = touch.nowPos;
			touch.nowPos = Vector2f((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
			it->onTouchEnded(win->getTouches(), win->getTouches().update(0));
			win->getTouches().remove(0);
		}
		touch = false;
	}
		break;
	case WM_MOUSEMOVE:
	{
		Interaction *it = app->getInteraction();
		if( touch && it != 0 ){
			TouchEvent::Touch &touch = win->getTouches().update(0);
			touch.phase = TouchEvent::TouchMoved;
			touch.lastPos = touch.nowPos;
			touch.nowPos = Vector2f((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
			it->onTouchMoved(win->getTouches(),win->getTouches().update(0));
			win->getTouches().update(0).phase = TouchEvent::TouchNormal;
		}
	}
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_KEYDOWN:
		break;
	case WM_SETFOCUS:
		//app->setFocus(true);
		return 0;
	case WM_KILLFOCUS:
		//app->setFocus(false);
		return 0;
	case WM_DESTROY:
		app->setDone();
		/*
	case WM_CLOSE:
		//pData->gShellDone = true;
		win->setDone();
	case WM_QUIT:
		return 0;*/
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


bool GLRenderWindow::create(long width, long height){
	WNDCLASS	wc;		
	DWORD		dwExStyle;
	DWORD		dwStyle;

	RECT WindowRect;
	WindowRect.left = 0l;
	WindowRect.top = 0l;

	WindowRect.right = width;
	WindowRect.bottom= height;

	HINSTANCE hInstance		= GetModuleHandle(NULL);			// Grab An Instance For Our Window

	wc.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// Redraw On Move, And Own DC For Window
	wc.lpfnWndProc	= (WNDPROC) _WndProc;				// WndProc Handles Messages
	wc.cbClsExtra	= 0;								// No Extra Window Data
	wc.cbWndExtra	= 0;								// No Extra Window Data
	wc.hInstance	= hInstance;						// Set The Instance
	wc.hIcon		= LoadIcon(NULL, IDI_WINLOGO);		// Load The Default Icon
	wc.hCursor		= LoadCursor(NULL, IDC_ARROW);		// Load The Arrow Pointer
	wc.hbrBackground	= NULL;							// No Background Required For GL
	wc.lpszMenuName		= NULL;							// We Don't Want A Menu
	wc.lpszClassName	= WNDCLASS_NAME;				// Set The Class Name


	if (!RegisterClass(&wc)){
		LOG_STAT( ERROR, "cannot register window class.");
		return false;
	}

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
	dwStyle = WS_OVERLAPPEDWINDOW|WS_VISIBLE;				// Windows Style

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	if (
		!(mWnd = CreateWindowEx(	dwExStyle,				// Extended Style For The Window
		WNDCLASS_NAME,				// Class Name
		"iPhone Simulator",		// Window Title
		WS_CLIPSIBLINGS |			// Required Window Style
		WS_CLIPCHILDREN |			// Required Window Style
		dwStyle						// Selected Window Style
		& ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME),	// no resize bar
		0, 0,						// Window Position
		WindowRect.right - WindowRect.left,	// Calculate Adjusted Window Width
		WindowRect.bottom- WindowRect.top,	// Calculate Adjusted Window Height
		NULL,						// No Parent Window
		NULL,						// No Menu
		hInstance,					// Instance
		this) )						// Pass self-pointer To WM_CREATE
		)
	{
		LOG_STAT( ERROR, "cannot create window.");
		return false;
	}

	return true;
}

bool GLRenderWindow::create(){
	float width(320.f), height(480.f);
	Device::getInstance()->getSize(width, height);

	return create((long)width,(long)height);
};

void GLRenderWindow::resize(bool){
	float width(320.f), height(480.f);
	Device::getInstance()->getSize(width, height);

	RECT rcClient, rcWindow;
	POINT ptDiff;
	GetClientRect(mWnd, &rcClient);
	GetWindowRect(mWnd, &rcWindow);
	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;

	mSize = Rectanglef( Vector2f(0.f,0.f), width, height );
	MoveWindow(mWnd,rcWindow.left, rcWindow.top, (LONG)width + ptDiff.x, (LONG)height + ptDiff.y, TRUE);
}

void GLRenderWindow::bind(){
	if( eglMakeCurrent(mEglDisplay, mEglWindow, mEglWindow, mEglContext) != EGL_TRUE)
		LOG_STAT( ERROR, "Failed to make current context." );
};

void GLRenderWindow::flush(){
	if( !eglSwapBuffers(mEglDisplay, mEglWindow) ){
	
		if(eglGetError() == EGL_CONTEXT_LOST){
			LOG_STAT( ERROR, "Lost context! Need to restart...bad luck." );
		}

		LOG_STAT( ERROR, "Unable to swap buffer." );
	}
};

bool GLRenderWindow::initContext(){

	// code followed is from ImgSdk and minimized.
	EGLint	majorVersion, minorVersion;

	mEglContext = 0;

	mHDC = GetDC( mWnd );
	mEglDisplay = eglGetDisplay( mHDC );

	if( mEglDisplay == EGL_NO_DISPLAY )
		mEglDisplay = eglGetDisplay((EGLNativeDisplayType)EGL_DEFAULT_DISPLAY);

	if(!eglInitialize(mEglDisplay, &majorVersion, &minorVersion)){
		LOG_STAT( ERROR, "Unable to initialize EGL." );
		return false;
	}

	LOG("EGL %d.%d initialized.", majorVersion, minorVersion);

	if(!eglBindAPI(EGL_OPENGL_ES_API)){
		LOG_STAT( ERROR, "Failed to bind OpenGL ES API." );
		return false;
	}

	mEglConfig = setEGLConfiguration();


	EGLint ai32ContextAttribs[32];
	int	i = 0;

	//ai32ContextAttribs[i++] = EGL_CONTEXT_CLIENT_VERSION;
	//ai32ContextAttribs[i++] = 2;

	ai32ContextAttribs[i] = EGL_NONE;

	mEglContext = eglCreateContext(mEglDisplay, mEglConfig, NULL, NULL);

	if( mEglContext == EGL_NO_CONTEXT ){
		LOG_STAT( ERROR, "Failed to create EGL Context." );
		return false;
	}

	EGLint		attrib_list[16];
	i = 0;
	attrib_list[i] = EGL_NONE;

	mEglWindow = eglCreateWindowSurface(mEglDisplay, mEglConfig, mWnd, NULL);
	
	if( mEglWindow == EGL_NO_SURFACE){
		LOG_STAT( ERROR, "Failed to create window surface." );
		return false;
	}

	if ( eglMakeCurrent(mEglDisplay, mEglWindow, mEglWindow, mEglContext) == EGL_TRUE )
		return eglSwapInterval(mEglDisplay, 1) == EGL_TRUE;
	else{
		LOG_STAT( ERROR, "Failed to make current context." );
		return false;
	}
}


EGLConfig GLRenderWindow::setEGLConfiguration()
{
    EGLint		num_config;
	EGLConfig	conf;

	// Setup the configuration list for our surface.
    EGLint conflist[] =
	{
		EGL_LEVEL, 0,
		EGL_NATIVE_RENDERABLE, EGL_FALSE,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
		EGL_CONFIG_CAVEAT, EGL_NONE,
		/*
			Tell it the minimum size we want for our colour buffer and the depth size so
			eglChooseConfig will choose the config that is the closest match.
		*/
		EGL_BUFFER_SIZE, 32,
		EGL_DEPTH_SIZE, 16,
		// The PBuffer bit is the important part as it shows we want a PBuffer
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
		EGL_BIND_TO_TEXTURE_RGBA, EGL_TRUE,
		EGL_SAMPLE_BUFFERS, 0,
		EGL_NONE
	};

	// Return null config if config is not found
    if(!eglChooseConfig(mEglDisplay, conflist, &conf, 1, &num_config) || num_config != 1){
		LOG_STAT( ERROR, "Unable to choose a valid config.." );
		return 0;
    }

	// Return config index
	return conf;
}

GLRenderWindow::~GLRenderWindow(){
	eglSwapBuffers(mEglDisplay, mEglWindow);
	eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
 	eglDestroyContext(mEglDisplay, mEglContext);
	eglDestroySurface(mEglDisplay, mEglWindow);
   	eglTerminate(mEglDisplay);
}