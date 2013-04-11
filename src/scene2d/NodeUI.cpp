#include "NodeUI.h"
#include "Scene2DManager.h"
#include "Scene2DNode.h"

#include "uielements.h"

#include <cstring>

//using namespace chaos;
using namespace ui;
using namespace Wm4;

#if 0
IMPLEMENT_CLASS(NodeUI, LIB_DOMAIN)
IMPLEMENT_PROP(handler, &NodeUI::setHandler, &NodeUI::getHandler)
IMPLEMENT_PROP(event, &NodeUI::setEventScpt, &NodeUI::getEventScpt)
IMPLEMENT_PROP(enabled, &NodeUI::setEnabled, &NodeUI::getEnabled)
IMPLEMENT_FUNC(intersect, &NodeUI::intersect)
IMPLEMENT_FUNC(test, &NodeUI::test)
IMPLEMENT_END;
#endif

ScriptState* NodeUI::sRunningState(0);

static void thdCallback(void* ud){
	*(ScriptState**)ud = 0;	
}

void NodeUI::intersect(Vector3f &pos) const{
	ASSERT(mNode->getFrame() != 0 && Scene2DManager::getInstance() != 0);
    
	Scene2DManager *scene = Scene2DManager::getInstance();
	Vector3f p(pos[0], pos[1], 0.f );
	Vector3f d(pos[0], pos[1], 1.f );
    
	scene->unproject(p);
	scene->unproject(d);
	(d-=p).Normalize();
    
    mNode->getFrame()->intersect(p, d, pos);
}

bool NodeUI::test(Vector3f const& pos) const{
	ASSERT(mNode->getFrame() != 0 && Scene2DManager::getInstance() != 0);
    
	Scene2DManager *scene = Scene2DManager::getInstance();
	Vector3f p(pos[0], pos[1], 0.f );
	Vector3f d(pos[0], pos[1], 1.f );
    
	scene->unproject(p);
	scene->unproject(d);
	(d-=p).Normalize();
    
    return mNode->getFrame()->test(p, d);
}

int NodeUI::testExtra( Vector2f const& pos,Vector3f& q, Vector2f const& extra){
	ASSERT(mNode->getFrame() != 0 && Scene2DManager::getInstance() != 0);

	Scene2DManager *scene = Scene2DManager::getInstance();
	// we extend the boundary a little bit to ease user interaction
	Vector3f p(pos[0], pos[1], 0.f );
	Vector3f d(pos[0], pos[1], 1.f );

	scene->unproject(p);
	scene->unproject(d);
	(d-=p).Normalize();

	return mNode->getFrame()->testIntersect(p,d,q,extra);
}

void NodeUI::parseElement(lua_State* L){
	int top = lua_gettop(L);
	lua_pushstring(L, "handler");
	lua_rawget(L, -2);
	if( lua_isfunction(L, -1) ){
		mHandler = TypeLua( lua_ref(L,1) );
		LOG("DEPRECATED: use bind instead. Default to MESSAGE event");
		getEvtHandlerRef().bind("message", mHandler);
	}

	lua_pushstring(L, "event");
	lua_rawget(L, top);
	if( lua_istable(L, -1) )
		setEventScpt(TypeLua(L));
	
	lua_pushstring(L, "disabled");
	lua_rawget(L, top);
	if( lua_isboolean(L,-1) && lua_toboolean(L,-1) == 1 )
		setEnabled( false );
	else
		setEnabled( true );

	lua_settop(L,top);
}

void NodeUI::setEventScpt(TypeLua const& lua){
	lua_State *L(lua.getL());
	int top = lua_gettop(L);
	lua.get(L);
	
	LOG("DEPRECATED: use bind UI event instead");
	if( lua_istable(L, -1) ){
		char const* keys[] = {"began", "moved", "ended", "cancelled", 0};
		char const* events[] = {"touchBegan", "touchMoved", "touchEnded", "touchCancelled", 0};
		TypeLua* scpt[] = {&mTouchBeganScpt, &mTouchMovedScpt, &mTouchEndedScpt, &mTouchCancelledScpt};
		for(char const** key = keys; *key != 0; ++key){
			lua_pushstring(L, *key);
			lua_rawget(L, top+1);
			if( lua_isfunction(L, -1) )
				getEvtHandlerRef().bind(events[key-keys], TypeLua(L));
				//*scpt[key-keys] = TypeLua(L);
			else{
				if( lua_isnil(L, -1) )
					getEvtHandlerRef().bind(events[key-keys], TypeLua());
					//*scpt[key-keys] = TypeLua();
				else
					LOG("Unable to set event for %s using (%s)", *key, luaL_typename(L,-1));
			}
		}
	}else {
		LOG("Unable to set event handler using (%s)", luaL_typename(L,-1));
	}

	lua_settop(L, top);
}

TypeLua NodeUI::getEventScpt() const{
	TypeLua lua;
	lua_State *L(lua.getL());
	int top = lua_gettop(L);

	char const* keys[] = {"began", "moved", "ended", "cancelled", 0};
	TypeLua const* scpt[] = {&mTouchBeganScpt, &mTouchMovedScpt, &mTouchEndedScpt, &mTouchCancelledScpt};
	for(char const** key = keys; *key != 0; ++key){
		lua_pushstring(L, *key);
		lua_getref(L, scpt[key-keys]->getRef());
		lua_rawset(L, -3);
	}
	
	lua_settop(L, top);
	
	return lua;
}

void NodeUI::setHandler( TypeLua const& lua) { 
	lua_State *L(lua.getL());
	lua.get(L);
	if( lua_isfunction(L,-1) )
		mHandler = lua;
	else{
		LOG("Unable to set a handler using (%s)", luaL_typename(L,-1));
	}
	lua_pop(L,1);
}

NodeUI* NodeUI::createUI(Scene2DNode* n, TypeLua const& lua){
	ASSERT( n != 0 );
	NodeUI* ui(0);
	lua_State *L(lua.getL());
	int top = lua_gettop(L);
	lua.get(L);

	ASSERT( lua_istable(L,-1) );
	lua_rawgeti(L,-1,1);

	if( lua_type(L,-1) == LUA_TSTRING) {
		char const* type = lua_tostring(L,-1);
		if( strcmp(type, "click") == 0 ){
			ui = new UIClick(n);
		}else if( strcmp(type, "check") == 0 ){
			ui = new UICheckBox(n);
		}else if( strcmp(type, "progress") == 0 ){
			ui = new UIProgress(n);
		}else if( strcmp(type, "rating") == 0 ){
			ui = new UIRating(n);
		}else if( strcmp(type, "counter") == 0 ){
			ui = new UICounter(n);
		}else
			LOG("Not a support ui element type.");
	}else if( lua_type(L,-1) == LUA_TUSERDATA){
		ScriptData *sd = (ScriptData*)lua_touserdata(L,-1);
		if( sd->dataType == ScriptData::Class && sd->class_ != 0 &&
			sd->class_->type == Type::CLASS)
		{
			Type *type(sd->class_->table->search(FACTORY_OP));
			if( type != 0 && type->type == Type::CONSTRUCTOR ){
				NodeFactory* factory = (NodeFactory*)type->factory;
				ui = (NodeUI*)factory->createNodeObject(n);
			}else
				LOG("Unable to find the factory method.");
		}

		if( ui == 0 || !ui->getClassType()->isDerived(TYPE(NodeUI) ) )
			LOG("Unable to create ui element. expected a ui class name. (%s)", ScriptManager::getInstance()->currentInfo().c_str());
	}else if( lua_isnoneornil(L,-1) ){
		ui = new NodeUI(n);
	}else
		LOG("No UI element's name specified.");

	if( ui != 0 ){
		lua_settop(L, top+1);
		ui->parseElement(L);
	}

	lua_settop(L, top);
	return ui;
}

bool NodeUI::runHandler(TypeLua const& lua){
	if( sRunningState != 0)
		return false;

	ScriptThread *ss = ScriptThread::thread( lua );

	if( ss && !ss->run(true) ){
		sRunningState = ss;
		Triggerable::byCallback( thdCallback, &sRunningState )->wait4( ss );
		//ss->addCallback( thdCallback, &sRunningState );
		return false;
	}else
		return true;
}

/*
void NodeUI::runEvent(TouchEvent &te, TouchEvent::Touch &t, TypeLua const& evt, Vector3f const& pos){
	if( sRunningState != 0 || evt.getRef() == -1)
		return;
	
	ScriptThread *ss = ScriptThread::thread( evt );
	
	if( ss && !ss->push_(pos[0])->push_(pos[1])->push_(pos[2])->run(true) ){
		sRunningState = ss;
		Triggerable::byCallback( thdCallback, &sRunningState )->wait4( ss );
		//ss->addCallback( thdCallback, &sRunningState );
	}
}*/

void NodeUI::sendMessage(long par1, long par2){
	Event *evt = new EventUIMessage(mNode, par1, par2);
	getEvtHandlerRef().queueEvent(evt);
	evt->release();
}

bool NodeUI::onMessage(Scene2DNode* from, void* data){
	if( mHandler.getRef() != -1 ){
		runHandler(mHandler);
		return true;
	}else 
		for(Scene2DNode* parent(mNode->getParent());
			parent != 0; parent = parent->getParent() )
		{
			if( parent->getUI() != 0 )
				return parent->getUI()->onMessage(from, data);
		}
	return false;
}
