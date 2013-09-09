#ifndef _IOSRENDERDRIVER_H
#define _IOSRENDERDRIVER_H

#include <common/common.h>
#include <UIKit/UIKit.h>
#include <re/RenderDriver.h>

_CHAOS_BEGIN

class RenderTarget;
class RenderWindow;

/**
 * Render driver for iOS, it only supports OpenGL ES
 * (collapse ES1 and ES2 to one class
 *
 * NB: other platforms can have more than one drivers 
 */
class iOSRenderDriver : public RenderDriver{
public:
    iOSRenderDriver(UIViewController*);
	~iOSRenderDriver();
    
	virtual RenderTarget* createTarget() {return 0;};
    virtual RenderWindow* createWindow(uint16_t width, uint16_t height);

private:
	UIViewController* _controller;
};

_CHAOS_END

#endif
