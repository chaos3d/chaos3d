/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_EVENTHANDLER_H
#define	_CHAOS_EVENTHANDLER_H

#include "common/common.h"
#include "event/Event.h"
#include <map>
#include <string>

_CHAOS_BEGIN

class CHAOS_API EventHandlerFunctor : public ReferencedCount{
public:
	virtual void call_(Event&) = 0;
};

class CHAOS_API EventHandler : public ReferencedCount, public ElementList<Event>{
	friend class Event;
public:
	//typedef void (*Handler)(Event&);
	typedef Event::EventType EventType;
	typedef EventHandlerFunctor* Handler;
	typedef std::map<EventType, Handler> TEventMap;
	// TODO: improve memory
	typedef std::map<std::string, EventType> TEventName;

private:
	static TEventName smEvtName;
protected:
	TEventMap mEvtMap;

	// start to process event queue
	virtual void processEvent(Event&);
public:
	EventHandler(){};
	virtual ~EventHandler();
	// get the current one
	// this is useful when manually chain the handlers, especially in the script
	Handler handlerFor(EventType type);
	// bind to an event, replace it if there is one
	void bind(EventType type, Handler);
	// helper function to bind a script to an evnt
	//void bind(char const*, TypeLua const& spt);
	// queue an event and process it
	// if there is on-going one, then it will wait until that is done
	void queueEvent(Event*);
	// queue a scripting event
	// in script, it shouldn't be allowed to create a native event
	//void queueEvent(TypeLua const&);

	// push new controller so it wil take over the control
	void pushHandler(EventHandler*);
	// pop itself and use previous one
	void popHandler();
};

//TYPE_RET_REF(EventHandler);

// TODO: class method can be cached
template<class D>
class EventHandlerFunction : public EventHandlerFunctor{
public:
	typedef void (D::*Handler)(Event&);
protected:
	D*	delegate;
	Handler handler;
public:
	EventHandlerFunction(D* d, Handler h) : delegate(d), handler(h) {};
	virtual void call_(Event& evt){
		(delegate->*handler)(evt);
	}
};

#define EVT_HANDLER(d, f)	HandlerResolver(d,f)
template<typename C, class F>
EventHandlerFunctor* HandlerResolver(C* d, F f){
	return (EventHandlerFunctor*)(new EventHandlerFunction<C>
		(d,(typename EventHandlerFunction<C>::Handler)f))->autorelease();
}

// for ease of programming
class EventNilHandler : public EventHandler{ 
public:
	virtual void processEvent(Event&) {};

	static EventNilHandler NIL;
};

class EventChainHandler : public EventHandler{

};

_CHAOS_END

#endif
