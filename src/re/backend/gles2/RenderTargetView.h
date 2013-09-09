#ifndef _RENDERTARGETVIEW_H
#define _RENDERTARGETVIEW_H

#include <common/common.h>
#include <re/RenderWindow.h>

// iOS only
#ifdef PLATFORM_IOS

#include <UIKit/UIKit.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

@class RenderView;

/**
 * render window for iOS
 */
class RenderTargetView : public RenderWindow {
public:
	/// default constructor
    // TODO: more options: full screen, position, color space
	RenderTargetView(UIView*,		//!< native shared context to create the window and multithreading (OpenGL concerns)
			uint16_t width = 0,     //!< window width
			uint16_t height = 0     //!< window height, default is the screen size
			);
	virtual ~RenderTargetView();

    virtual Size getSize() const;
	virtual int getColorFormat() const;
	virtual int getDepthFormat() const;
	virtual int getStencilFormat() const;
    
	virtual void bind();
	virtual void flush();
	virtual void screenshot();

private:
    void createBuffers();
    void deleteBuffers();
    
	RenderView* _view;	// TODO: customized view
    GLuint _resolveFrameBuffer;
    GLuint _sampleFrameBuffer;
    GLuint _colorRenderBuffer;
    GLuint _depthRenderBuffer;
    GLuint _multisampleFrameBuffer;
};

#endif // PLATFORM_IOS

#endif
