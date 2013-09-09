#ifndef _RENDERDRIVER_H
#define _RENDERDRIVER_H

#include <common/common.h>

_CHAOS_BEGIN

class RenderTarget;
class RenderWindow;

/// This might not be needed since per-OS inheritant
/// doesn't really make sense rather than a plain
/// class implemation for different OS
/// whereas those OS that can create more than one
/// drivers (OGL/DX) could have a separate parent
/// class
///
/// correct: this is good to just expose the interface
/// instead of all implementation details and dependencies

/*
 * Abstract factory for creating render resources
 */
struct DriverCap{
	uint8_t max_texture;
};

class RenderDriver{
public:
	virtual RenderTarget* createTarget() = 0;
	virtual void getCap(DriverCap&) = 0;
    virtual RenderWindow* createWindow(uint16_t width, uint16_t height) = 0;
};

_CHAOS_END

#endif