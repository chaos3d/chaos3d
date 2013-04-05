/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_SCENE2DHANDLER_H
#define	_CHAOS_SCENE2DHANDLER_H

#include "chaos_config.h"
#include "core/core.h"
#include "event/evt.h"
#include <map>

_CHAOS_BEGIN

class Scene2DNode;
class Scene2DManager;

// TODO: add new handler for UI, that propagate the message to its parent
/**
 * Scene handler
 * pick up a node that has ui component, and track all 
 * the touch events for this node
 * */
class CHAOS_API Scene2DHandler : public EventHandler{
public:
	typedef struct Track{
		Scene2DNode* node;
		Track() : node(0) {};
	} TTrack;
	typedef std::map<int, TTrack> TTrackMap;

protected:
	Scene2DManager* mScene;
	TTrackMap mTkMap;

	void touchBegan(EventTouchBegan&);
	void touchMoved(EventTouchMoved&);
	void touchEnded(EventTouchEnded&);
	void touchCancelled(EventTouchCancelled&);

public:
	Scene2DHandler(Scene2DManager* sc);
};


_CHAOS_END

#endif
