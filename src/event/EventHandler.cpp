#include "EventHandler.h"
#include "script/scripting.h"
#include "script/bind.h"

//using namespace chaos;

_CHAOS_BEGIN

// script handler wrapper
// return true if script process the event
class CHAOS_PRIVATE EventHandlerScript : public EventHandlerFunctor{
protected:
	TypeLua	mLua;
public:
	EventHandlerScript(TypeLua l) : mLua(l) {};
	virtual void call_(Event& evt);
};

_CHAOS_END

EventNilHandler EventNilHandler::NIL;

IMPLEMENT_CLASS(EventHandler, LIB_DOMAIN)
IMPLEMENT_FUNC(bind, (void (EventHandler::*)(char const*,TypeLua const&))&EventHandler::bind)
IMPLEMENT_FUNC(queue, (void (EventHandler::*)(TypeLua const&))&EventHandler::queueEvent)
IMPLEMENT_FUNC(push, &EventHandler::pushHandler)
IMPLEMENT_FUNC(pop, &EventHandler::popHandler)
IMPLEMENT_END

EventHandler::TEventName EventHandler::smEvtName;
EventHandler::~EventHandler(){
	for(TEventMap::iterator it = mEvtMap.begin(); it != mEvtMap.end(); ++it){
		SAFE_RELEASE0(it->second);
	}
}

void EventHandler::bind(EventType type, Handler handler) {
	SAFE_REFAGN(mEvtMap[type], handler);
}

void EventHandler::bind(char const* typeName, TypeLua const& spt) {
	TEventName::iterator it = Event::smEvtName->find(std::string(typeName));
	if( it == Event::smEvtName->end() ){
		LOG("Event (%s) not defined, ignore", typeName);
		return;
	}

	EventType type = it->second;
	Handler handler = new EventHandlerScript(spt);
	SAFE_REFAGN(mEvtMap[type], handler);
	handler->release();
}

EventHandler::Handler EventHandler::handlerFor(EventType type) {
	TEventMap::iterator it = mEvtMap.find(type);
	if( it != mEvtMap.end() )
		return it->second;
	else
		return 0;
}

void EventHandler::queueEvent(TypeLua const& lua){
	queueEvent((Event*)(new EventScript(lua))->autorelease());
}

void EventHandler::queueEvent(Event* evt){
	// first one? then start processing it
	if(empty()) {
		append(evt);
		for (iterator it = last(); it;) {
			processEvent(*it);
			remove(&(*it--));
		}
	} else {	// otherwise, queue it and process after
		append(evt);
	}
}

void EventHandler::processEvent(Event& evt) {
	TEventMap::iterator it = mEvtMap.find(evt.getType());
	if( it != mEvtMap.end() && it->second != 0){
		it->second->call_(evt);
	}
}

// TODO
void EventHandler::pushHandler(EventHandler* handler){

}

void EventHandler::popHandler(){

}

void EventHandlerScript::call_(Event& evt) {
	ScriptThread* thread = ScriptThread::thread(mLua);
	if(thread == 0)
		return;
	thread->push_(&evt);
	if(thread->run(true) && thread->get_<bool>(1))
		evt.processed();
}
