/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_NODEUI_H
#define	_CHAOS_NODEUI_H

#include "common/common.h"
#include "wm4-algebra/algebra.h"
//#include "EventUI.h"

_CHAOS_BEGIN

class Scene2DNode;

class NodeUI{
protected:
//	static ScriptState*	sRunningState;
//	TypeLua	mHandler;
//	TypeLua mTouchBeganScpt, mTouchMovedScpt, mTouchEndedScpt, mTouchCancelledScpt;
//	EventHandler* mEvtHandler;
	bool	mEnabled;

//	virtual void parseElement(lua_State* L);
	virtual bool onMessage(Scene2DNode* from, void* data = 0);
//	bool	runHandler(TypeLua const&);
	void sendMessage(long par1 = 0L, long par2 = 0L);

public:
	NodeUI(Scene2DNode* n);// : NodeComponent(n),mEvtHandler(new EventHandler()), mEnabled(true) {};
//	virtual ~NodeUI() { SAFE_RELEASE0(mEvtHandler); };
//	void intersect(Vector3f &) const;
	bool test(Wm4::Vector3f const&) const;
	int testExtra(Wm4::Vector2f const&, Wm4::Vector3f& q, Wm4::Vector2f const& );

#if 0
	EventHandler&	getEvtHandlerRef() { return mEvtHandler ? *mEvtHandler : EventNilHandler::NIL; }
	EventHandler*	getEvtHandler() const { return mEvtHandler; }
	void setEvtHandler(EventHandler* handler) { SAFE_REFAGN(mEvtHandler, handler); }

	void setHandler(TypeLua const& lua);
	TypeLua const& getHandler() const {return mHandler;}
	
	void setEventScpt(TypeLua const& lua);
	TypeLua getEventScpt() const;
#endif
	void	setEnabled(bool enable) { mEnabled = enable; };
	bool	getEnabled() const { return mEnabled; }

//	static NodeUI* createUI(Scene2DNode*, TypeLua const&);
};

_CHAOS_END

#endif
