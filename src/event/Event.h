/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_EVENT_H
#define	_CHAOS_EVENT_H

#include "common/common.h"
#include "common/ElementList.h"
#include "common/ReferencedCount.h"
#include "wm4-algebra/algebra.h"

_CHAOS_BEGIN

class EventHandler;
class Event : public ReferencedCount, public Element{
public:
	typedef int EventType;

protected:
    template<class E>
    class EventTag {
    public:
        EventTag(EventType t) { _tag = Event::newEvtType(); }
        operator const EventType&() const { return _tag; }
    private:
        EventType _tag;
    };
    
protected:
	// if further propagated
	bool	mProcessed;

public:
	Event(EventType t) : mProcessed(false), mType(t) {};
	EventType getType() const { return mType; };
	
	void processed() { mProcessed = true; }
	bool isProcessed() { return mProcessed; }

	// TODO
	// note: the memory queue will be a multiply of preasumble size
	// the event alloc and dealloc will be queued, since it will be
	// always used in first-in-last-out order
	//static void* operator new(size_t);
	//static void operator delete(void*);
    
private:
	static EventType newEvtType(char const* name = 0){
        static EventType _startEvt = 10000;
        return ++_startEvt;
    }
	
	EventType mType;
    
    friend class EventHandler;
};

#define DEFINE_EVENT(claz)	\
	public: EventTag<claz> TYPE

#define EVT_TYPE(claz)	\
	((Event::EventType)claz::TYPE)

/*
class EventScript: public Event {
	DEFINE_EVENT(EventScript);
protected:
	TypeLua	mScript;
	
public:
	explicit EventScript(TypeLua const& spt) : mScript(spt), Event(EVT_TYPE(EventScript))
	{}
	
	TypeLua const& getScript() const {
		return mScript;
	}
};
*/

class EventTouch : public Event {
public:
	// TODO: make them protected
    Wm4::Vector2f pos;
	float time;
	int tap;
	int which;	// identify multiple touches

public:
	EventTouch(Wm4::Vector2f const& p, float t, int tap_, EventType ty)
		: pos(p), time(t), tap(tap_), Event(ty)
	{}

	bool same(EventTouch const& rhs){
		return rhs.which == which;
	}
};

class EventTouchBegan : public EventTouch{
	DEFINE_EVENT(EventTouchBegan);
public:
	EventTouchBegan(Wm4::Vector2f const& p, float t, int tap_)
		: EventTouch(p,t,tap_,EVT_TYPE(EventTouchBegan))
	{}
};

class EventTouchMoved : public EventTouch{
	DEFINE_EVENT(EventTouchMoved);
public:
	EventTouchMoved(Wm4::Vector2f const& p, float t, int tap_)
		: EventTouch(p,t,tap_,EVT_TYPE(EventTouchMoved))
	{}
};

class EventTouchEnded : public EventTouch{
	DEFINE_EVENT(EventTouchEnded);
public:
	EventTouchEnded(Wm4::Vector2f const& p, float t, int tap_)
		: EventTouch(p,t,tap_,EVT_TYPE(EventTouchEnded))
	{}
};

class EventTouchCancelled : public EventTouch{
	DEFINE_EVENT(EventTouchCancelled);
public:
	EventTouchCancelled(Wm4::Vector2f const& p, float t, int tap_)
		: EventTouch(p,t,tap_,EVT_TYPE(EventTouchCancelled))
	{}
};

_CHAOS_END

#endif
