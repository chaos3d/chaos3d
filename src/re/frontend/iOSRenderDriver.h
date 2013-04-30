#ifndef _IOSRENDERDRIVER_H
#define _IOSRENDERDRIVER_H

#include <re/RenderDriver.h>

/**
 * Render driver for iOS, it only supports OpenGL ES
 * (collapse ES1 and ES2 to one class
 *
 * NB: other platforms can have more than one drivers 
 */
class iOSRenderDriver : public RenderDriver {

};

#endif
