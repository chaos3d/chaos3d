/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_EVENTUI_H
#define	_CHAOS_EVENTUI_H

#include "chaos_config.h"
#include "core/math.h"
#include "core/RTTI.h"
#include "event/Event.h"
#include "2d/Scene2DNode.h"

_CHAOS_BEGIN

/**
 * a generic ui message that contains two fields
 */
DECLARE_CLASS(EventUIMessage, Event);
class CHAOS_API EventUIMessage : public Event{
	DECLARE_EVENT;
protected:
	Scene2DNode *node;	// where it is from
	long param1, param2;	// two generic fields, can save any data
	
public:
	EventUIMessage(Scene2DNode* n, long par = 0L, long par2 = 0L) 
	: param1(par), node(n), param2(par2), Event(EVT_TYPE(EventUIMessage))
	{	SAFE_RETAIN(node);
	};
	
	virtual ~EventUIMessage() {
		SAFE_RELEASE0(node);
	}
	
	Scene2DNode* getNode() const { return node; }

	template<typename T>
	T getParameter(bool second = false) const{
		return *(T*)(&(second?param2:param1));
	}
};

#define MSG_CONVERT(type, par)	*(type*)&(par)

class CHAOS_API EventUI : public Event{
	// TODO: make it protected
public:
	Vector3f pos;
	Scene2DNode *node;
	Vector2f tpos;	// touch pos

public:
	EventUI(Scene2DNode* n, Vector3f const& p, Vector2f const& tp, EventType t) 
		: pos(p), node(n), tpos(tp), Event(t)
	{	SAFE_RETAIN(node);
	};

	virtual ~EventUI() {
		SAFE_RELEASE0(node);
	}

	Vector3f& getPos() { return pos; }
	Scene2DNode* getNode() const { return node; }
	Vector3f const& getPos() const { return pos; };
};

class CHAOS_API EventUITouchBegan : public EventUI{
	DECLARE_EVENT;
public:
	EventUITouchBegan(Scene2DNode* n,Vector3f const& p,Vector2f const& tp) 
		: EventUI(n,p,tp,EVT_TYPE(EventUITouchBegan)) {}
};

class CHAOS_API EventUITouchMoved: public EventUI{
	DECLARE_EVENT;
public:
	// TODO retain scene node and make it protected
	Scene2DNode *beganNode;
	EventUITouchMoved(Scene2DNode* n,Vector3f const& p,Vector2f const& tp)
		: EventUI(n,p,tp,EVT_TYPE(EventUITouchMoved)), beganNode(0){}
	~EventUITouchMoved() {
	}
};

class CHAOS_API EventUITouchEnded : public EventUI{
	DECLARE_EVENT;
public:
	Scene2DNode *beganNode;
	EventUITouchEnded(Scene2DNode* n,Vector3f const& p,Vector2f const& tp)
		: EventUI(n,p,tp,EVT_TYPE(EventUITouchEnded)), beganNode(0) 
	{}
};

class CHAOS_API EventUITouchCancelled : public EventUI{
	DECLARE_EVENT;
public:
	Scene2DNode *beganNode;
	EventUITouchCancelled(Scene2DNode* n,Vector3f const& p,Vector2f const& tp)
		: EventUI(n,p,tp,EVT_TYPE(EventUITouchCancelled)), beganNode(0)
	{}
};

_CHAOS_END

#endif
