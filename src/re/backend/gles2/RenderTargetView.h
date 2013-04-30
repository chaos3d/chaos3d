#ifndef _RENDERTARGETVIEW_H
#define _RENDERTARGETVIEW_H

#include <common/common.h>
#include <re/RenderWindow.h>

// iOS only
#if __APPLE__
#include <TargetConditionals.h>
#endif

#if TARGET_OS_IPHONE == 1 || TARGET_IPHONE_SIMULATOR == 1

#include <UIKit/UIKit.h>

/**
 * render window for iOS
 */
class RenderTargetView : public RenderWindow {
public:
	/// the native render context that API relies on
	/// this is ios implementation
	class Context;

	/// default constructor
	RenderTargetView(Context*,		//!< native shared context to create the window and multithreading (OpenGL concerns)
			uint16_t width = 0,	//!< window width
			uint16_t height = 0 //!< window height, default is the screen size
			);
	virtual ~RenderTargetView();

private:
	UIView* _view;	// TODO: customized view
};

#endif

#endif
