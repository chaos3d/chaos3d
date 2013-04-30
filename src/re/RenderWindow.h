#ifndef _RENDERWINDOW_H
#define _RENDERWINDOW_H

#include <common/common.h>
//#include <event/EventHandler.h>
#include "RenderDriver.h"
#include "RenderTarget.h"

_CHAOS_BEGIN
class EventHandler;// TODO
/**
 * RenderWindow is a glue between render target and
 * the native window regarding to the user interaction
 * etc.
 *
 */
class RenderWindow : public RenderTarget{
	typedef EventHandler Handler;

public:
	/// the event handler for mouse/touches etc.
	Handler* getHandler() const { return _handler; }

private:
	Handler* _handler;
};

_CHAOS_END

#endif
