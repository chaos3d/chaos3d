#include "2d/Scene2DHandler.h"
#include "2d/scene.h"
#include "2d/NodeUI.h"

//using namespace chaos;

Scene2DHandler::Scene2DHandler(Scene2DManager *sc) : mScene(sc) {
	bind(EVT_TYPE(EventTouchBegan), EVT_HANDLER(this,&Scene2DHandler::touchBegan));
	bind(EVT_TYPE(EventTouchMoved), EVT_HANDLER(this,&Scene2DHandler::touchMoved));
	bind(EVT_TYPE(EventTouchEnded), EVT_HANDLER(this,&Scene2DHandler::touchEnded));
	bind(EVT_TYPE(EventTouchCancelled), EVT_HANDLER(this,&Scene2DHandler::touchCancelled));
}

void Scene2DHandler::touchBegan(EventTouchBegan& evt){
	// TODO: remove hit_ret
	int ret = 0;
	Vector3f pos;
	Scene2DNode* node = mScene->pick(evt.pos, pos, ret);

	// if ui is disabled, then pass to its first enabled parent
	while( node !=0 && node->getUI() != 0 &&
			!node->getUI()->getEnabled() )
		node = node->getParent();

	if( node != 0 && node->getUI() != 0 ){
		mTkMap[evt.which].node = node;
		EventUITouchBegan* uiEvt = new EventUITouchBegan(node,pos,evt.pos);
		node->getUI()->getEvtHandlerRef().queueEvent(uiEvt);
		uiEvt->release();
		evt.processed();
	}
}

void Scene2DHandler::touchMoved(EventTouchMoved& evt){
	// only process tracked node
	TTrackMap::iterator it = mTkMap.find(evt.which);
	if( it == mTkMap.end() && it->second.node != 0)
		return;

	Vector3f pos;
	Scene2DNode* node = 0; //mScene->pick(evt.pos, pos, ret);
	Scene2DNode* bnode = it->second.node;
	EventUITouchMoved* uiEvt = new EventUITouchMoved(node,pos,evt.pos);
	uiEvt->beganNode = bnode;
	bnode->getUI()->getEvtHandlerRef().queueEvent(uiEvt);
	uiEvt->release();
	evt.processed();
}

void Scene2DHandler::touchEnded(EventTouchEnded& evt){
	// only process tracked node
	TTrackMap::iterator it = mTkMap.find(evt.which);
	if( it == mTkMap.end() && it->second.node != 0)
		return;

	Vector3f pos;
	Scene2DNode* node = 0; //mScene->pick(evt.pos, pos, ret);
	Scene2DNode* bnode = it->second.node;
	EventUITouchEnded* uiEvt = new EventUITouchEnded(node,pos,evt.pos);
	uiEvt->beganNode = bnode;
	bnode->getUI()->getEvtHandlerRef().queueEvent(uiEvt);
	uiEvt->release();
	evt.processed();
	mTkMap.erase(it);
}

void Scene2DHandler::touchCancelled(EventTouchCancelled& evt){
	// only process tracked node
	TTrackMap::iterator it = mTkMap.find(evt.which);
	if( it == mTkMap.end() && it->second.node != 0)
		return;

	Vector3f pos;
	Scene2DNode* node = 0; //mScene->pick(evt.pos, pos, ret);
	Scene2DNode* bnode = it->second.node;
	EventUITouchCancelled* uiEvt = new EventUITouchCancelled(node,pos,evt.pos);
	uiEvt->beganNode = bnode;
	bnode->getUI()->getEvtHandlerRef().queueEvent(uiEvt);
	uiEvt->release();
	evt.processed();
	mTkMap.erase(it);
}
