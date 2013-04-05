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

#include "chaos_config.h"
#include "core/math.h"
#include "core/RTTI.h"
#include "event/evt.h"
#include "script/LuaType.h"
#include "script/scripting.h"
#include "2d/NodeComponent.h"
#include "2d/EventUI.h"

struct lua_State;

_CHAOS_BEGIN

class Scene2DNode;

DECLARE_CLASS(NodeUI, NilParent);
class NodeUI : public NodeComponent{
	DYNAMIC_CLASS(NodeUI);

protected:
	static ScriptState*	sRunningState;
	TypeLua	mHandler;
	TypeLua mTouchBeganScpt, mTouchMovedScpt, mTouchEndedScpt, mTouchCancelledScpt;
	EventHandler* mEvtHandler;
	bool	mEnabled;

	virtual void parseElement(lua_State* L);
	virtual bool onMessage(Scene2DNode* from, void* data = 0);
	bool	runHandler(TypeLua const&);
	void sendMessage(long par1 = 0L, long par2 = 0L);

public:
	NodeUI(Scene2DNode* n) : NodeComponent(n),mEvtHandler(new EventHandler()), mEnabled(true) {};
	virtual ~NodeUI() { SAFE_RELEASE0(mEvtHandler); };
	void intersect(Vector3f &) const;
	bool test(Vector3f const&) const;
	int testExtra(Vector2f const&,Vector3f& q, Vector2f const& );

	EventHandler&	getEvtHandlerRef() { return mEvtHandler ? *mEvtHandler : EventNilHandler::NIL; }
	EventHandler*	getEvtHandler() const { return mEvtHandler; }
	void setEvtHandler(EventHandler* handler) { SAFE_REFAGN(mEvtHandler, handler); }

	void setHandler(TypeLua const& lua);
	TypeLua const& getHandler() const {return mHandler;}
	
	void setEventScpt(TypeLua const& lua);
	TypeLua getEventScpt() const;

	void	setEnabled(bool enable) { mEnabled = enable; };
	bool	getEnabled() const { return mEnabled; }

	static NodeUI* createUI(Scene2DNode*, TypeLua const&);
};

TYPE_RET_DYN(NodeUI);

_CHAOS_END

#endif
