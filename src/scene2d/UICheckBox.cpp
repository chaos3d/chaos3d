#include "2d/UICheckBox.h"

#include "2d/Transform.h"
#include "script/lua.h"
#include "script/bind.h"
#include "script/ScriptData.h"
#include "2d/Scene2DManager.h"
#include "2d/Scene2DNode.h"

using namespace ui;
//using namespace chaos;

IMPLEMENT_CLASS(UICheckBox, LIB_DOMAIN )
IMPLEMENT_PROP(checked, &UICheckBox::setChecked, &UICheckBox::getChecked)
IMPLEMENT_END

UICheckBox::UICheckBox(Scene2DNode* n) : NodeUI(n), mCkNode(0), mChecked(false){
	mEvtHandler->bind(EVT_TYPE(EventUITouchEnded), EVT_HANDLER(this,&UICheckBox::touchEnded));
}

UICheckBox::~UICheckBox(){
	if( mCkNode != 0 )
		mCkNode->release();
}

void UICheckBox::touchEnded(EventUITouchEnded& evt){
	if(evt.beganNode != mNode)
		return;
	
	Vector3f q;
	if( testExtra(evt.tpos, q, Vector2f(16.f,16.f)) != 0 ){
		setChecked(!mChecked);
		sendMessage(MSG_CONVERT(bool, mChecked));
	}
	evt.processed();
}

void UICheckBox::setChecked(bool c){
	mChecked = c;
	if( mChecked )
		mNode->addChild( mCkNode );
	else
		mCkNode->removeSelf();
}

void UICheckBox::parseElement(lua_State* L){
	int top = lua_gettop(L);

	lua_pushstring(L, "node");
	lua_rawget(L, -2);
	if( lua_istable(L,-1) ){
		ASSERT(Scene2DManager::getInstance() != 0);
		mCkNode = Scene2DManager::getInstance()->createNode(TypeLua(L));
	}else if( lua_isuserdata(L,-1) ){
		ScriptData* data = (ScriptData*)lua_touserdata(L, -1);
		if( data != 0 && data->dataType == ScriptData::Instance &&
			data->type->isDerived(TYPE(Scene2DNode)))
			mCkNode = (Scene2DNode*)data->obj;
	}else
		LOG("No check node defined.");

	if( mCkNode != 0 )
		mCkNode->retain();

	lua_settop(L,top);
	lua_pushstring(L, "check");
	lua_rawget(L, -2);
	if( lua_isboolean(L,-1) && lua_toboolean(L,-1) )
		setChecked(true);

	lua_settop(L,top);
	return NodeUI::parseElement(L);
};
