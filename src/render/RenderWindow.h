/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_RENDERWINDOW_H
#define	_CHAOS_RENDERWINDOW_H

#include "chaos_config.h"
#include "RenderTarget.h"
#include "event/EventHandler.h"

_CHAOS_BEGIN

class RenderWindow : public RenderTarget{
protected:
	EventHandler*	mInputHandler;

public:
	enum{
		PortraitDown = 0,
		PortraitUp = 1,
		LandscapeLeft = 2,
		LandscapeRight = 3,
	};

	RenderWindow() : mInputHandler(0){};
	~RenderWindow() { SAFE_RELEASE0(mInputHandler); };
	void setHandler(EventHandler* evt) { SAFE_REFAGN(mInputHandler, evt); };
	EventHandler& getHandler() const { return mInputHandler ? *mInputHandler : EventNilHandler::NIL; };
	virtual void resize(bool anim) = 0;
};

_CHAOS_END

#endif

