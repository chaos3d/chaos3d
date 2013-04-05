#include "2d/UIClick.h"
#include "2d/Scene2DNode.h"
#include "2d/Transform.h"
#include "script/lua.h"
#include "script/ScriptData.h"
#include "2d/NodeApplier.h"

//using namespace chaos;
using namespace ui;

Vector3fAnimation UIClick::sScaleAnim(false, 
									  &Vector3fAnimation::KeyFrame(Vector3f(.95f,.95f,.95f),1.f),
									  &Vector3fAnimation::KeyFrame(Vector3f(1.f,1.f,1.f),0.f),0);

UIClick::~UIClick(){
	if(mClickAnim != 0 )
		mClickAnim->release();
}

UIClick::UIClick(Scene2DNode* n) : NodeUI(n), mClickAnim(0), mReplaced(false) {
	ASSERT( n != 0 );
	if( mNode->getTransform() != 0 ){
		setClickAnim(mNode->getTransform()->animation<Transform::ScaleApplier, LinearTiming>( &sScaleAnim ));
	}
	
	mEvtHandler->bind(EVT_TYPE(EventUITouchBegan), EVT_HANDLER(this,&UIClick::touchBegan));
	mEvtHandler->bind(EVT_TYPE(EventUITouchEnded), EVT_HANDLER(this,&UIClick::touchEnded));
}

void UIClick::touchBegan(EventUITouchBegan& evt){
	if( mClickAnim != 0 )
		mClickAnim->play(0.f,.2f,1.f);
	else if( mReplaced ){
		if( mRectClick.canvas != 0 )
			mNode->getSprite()->setCanvas(mRectClick.canvas);
		mNode->getSprite()->setBound(mRectClick.x, mRectClick.y, 
									 mRectClick.width, mRectClick.height);
	}
	
	evt.processed();
}

void UIClick::touchEnded(EventUITouchEnded& evt){
	if( evt.beganNode->getUI() != this )
		return;
	
	if( mClickAnim != 0 )
		mClickAnim->play(0.f,-.2f,1.f);
	else if( mReplaced ){
		if( mRectOrig.canvas != 0 )
			mNode->getSprite()->setCanvas(mRectOrig.canvas);
		mNode->getSprite()->setBound(mRectOrig.x, mRectOrig.y, 
									 mRectOrig.width, mRectOrig.height);
	}
	
	Vector3f q;
	if( testExtra(evt.tpos, q, Vector2f(16.f,16.f)) != 0 ){
		sendMessage();
		//onMessage( mNode );
		evt.processed();
	}
}

void UIClick::setClickAnim(AnimationState* anim){
	if( anim != 0 )
		anim->retain();

	if( mClickAnim != 0 )
		mClickAnim->release();

	mClickAnim = anim;
}

void UIClick::parseElement(lua_State* L){
	int top = lua_gettop(L);

	lua_pushstring(L, "anim");
	lua_rawget(L, -2);
	if( lua_isboolean(L,-1) && lua_toboolean(L,-1) == 0 )
		setClickAnim(0);
	else while(lua_istable(L, -1)){
		lua_rawgeti(L, -1, 2);	// animation
		ScriptData* data = (ScriptData*)lua_touserdata(L, -1);
		if( data == 0 || data->dataType != ScriptData::Instance )
			break;

		lua_rawgeti(L, -2, 1);
		char const* type = lua_tostring(L,-1);
		if( strcmp( type, "scale") == 0 ){
			if( mNode->getTransform() != 0 )
				setClickAnim(mNode->getTransform()->animation<Transform::ScaleApplier, LinearTiming>( (Vector3fAnimation*)data->obj ));
		}else if( strcmp( type, "color") == 0 ){
			/*
			if( mNode->getColor() != 0 )
				setClickAnim(mNode->getTransform()->animation<Transform::ScaleApplier, LinearTiming>( (ColorAnimation*)data->obj ));
				*/
		}else if( strcmp( type, "rotate") == 0 ){
			if( mNode->getTransform() != 0 )
				setClickAnim(mNode->getTransform()->animation<Transform::RotateApplier, LinearTiming>( (QuaternionAnimation*)data->obj ));
		}else if( strcmp( type, "translate") == 0 ){
			if( mNode->getTransform() != 0 )
				setClickAnim(mNode->getTransform()->animation<Transform::TranslateApplier, LinearTiming>( (Vector3fAnimation*)data->obj ));
		}

		break;
	}

	lua_pushstring(L, "replace");
	lua_rawget(L, top);
	while( lua_istable(L,-1) ){
		if( mNode->getSprite() == 0 || mNode->getSprite()->getCanvas() == 0 ){
			LOG("Sprite is not set up correctly. (UIClick)");
			break;
		}

		Rectanglef const& bound(mNode->getSprite()->getBound());
		mRectOrig = Sprite::SpriteValue( bound.Origin[0], bound.Origin[1],
			bound.Extent[0], bound.Extent[1], mNode->getSprite()->getCanvas() );
		
		LUA_GET4(L)	;
		Sprite::SpriteValue value((float)lua_tonumber(L, -4),
			(float)lua_tonumber(L, -3),
			(float)lua_tonumber(L, -2),
			( float)lua_tonumber(L, -1), 0);

		lua_rawgeti( L, -5, 5);
		ScriptData* data = (ScriptData*)lua_touserdata(L, -1);
		if( data != 0 && data->obj != 0 ){
			if( data->dataType != ScriptData::Instance ) //todo type checking
				LOG("Canvas not applicable. (UIClick)");
			else
				value.canvas = (Canvas*)data->obj;
		}
		mRectClick = value;

		if( mClickAnim != 0 ){
			LOG("Use sprite replacement instead of animations. (UIClick)");
			setClickAnim(0);
		}

		mReplaced = true;
		break;
	}

	lua_settop(L,top);
	return NodeUI::parseElement(L);
};
