#include "2d/UIProgress.h"
#include "script/lua.h"
#include "script/bind.h"
#include "2d/Scene2DNode.h"
#include "2d/PartialSprite.h"
#include "anim/anim.h"

using namespace ui;
//using namespace chaos;

IMPLEMENT_CLASS(UIProgress, LIB_DOMAIN )
IMPLEMENT_PROP(percent, &UIProgress::setPercent, &UIProgress::getPercent)
IMPLEMENT_PROP(step, &UIProgress::setStep, &UIProgress::getStep)
IMPLEMENT_FUNC(animation, &UIProgress::animation)
IMPLEMENT_END

UIProgress::UIProgress(Scene2DNode* n) : NodeUI(n),
	mPercent(1.f), mStep(0.f)
{
	ASSERT( mNode != 0 && mNode->getSprite() != 0);
	mEvtHandler->bind(EVT_TYPE(EventUITouchBegan), EVT_HANDLER(this,&UIProgress::touchBegan));
	mEvtHandler->bind(EVT_TYPE(EventUITouchMoved), EVT_HANDLER(this,&UIProgress::touchMoved));
	mEvtHandler->bind(EVT_TYPE(EventUITouchEnded), EVT_HANDLER(this,&UIProgress::touchEnded));
}

void UIProgress::parseElement(lua_State* L){
	int top = lua_gettop(L);

	lua_pushstring(L, "percent");
	lua_rawget(L, -2);
	if( lua_isnumber(L,-1) )
		setPercent((float)lua_tonumber(L,-1));

	lua_pushstring(L, "step");
	lua_rawget(L, top);
	if( lua_isnumber(L,-1) )
		setStep((float)lua_tonumber(L,-1));
	
	lua_settop(L,top);
	return NodeUI::parseElement(L);
};

void UIProgress::setPercent(float percent){
	if( mStep > FLT_EPSILON ){
		percent += mStep*.9f;	// ceilling
		percent = percent - fmod( percent, mStep );
	}

	if( percent < 0.f )
		percent = 0.f;
	else if( percent > 1.f )
		percent = 1.f;

	((PartialSprite*)mNode->getSprite())->setPercent( mPercent = percent );
}

void UIProgress::setStep(float step){
	if( step > 1.f )
		step = 1.f;
	else if( step < FLT_EPSILON )
		step = 0.f;
	mStep = step;
}

AnimationState*	UIProgress::animation(float percent){
	AnimationState* as(0);
	ValueAnimation* va = new ValueAnimation(false, &ValueAnimation::KeyFrame(mPercent, 0.f),
		&ValueAnimation::KeyFrame(percent, 1.f), 0);

	as = va->createState<PercentApplier,LinearTiming>(PercentApplier(this));
	va->release();
	return as;
}

void UIProgress::touchBegan(EventUITouchBegan& evt){
	Rectanglef const& frame( ((PartialSprite*)mNode->getSprite())->getOriginFrame() );
	setPercent((evt.pos[0]-frame.Origin[0])/frame.Extent[0]);
	evt.processed();
}

void UIProgress::touchMoved(EventUITouchMoved& evt){
	if(evt.beganNode != mNode)
		return;

	Vector3f q;
	testExtra(evt.tpos, q, Vector2f(0.f,0.f));
	Rectanglef const& frame( ((PartialSprite*)mNode->getSprite())->getOriginFrame() );
	setPercent((q[0]-frame.Origin[0])/frame.Extent[0]);
	evt.processed();
}

void UIProgress::touchEnded(EventUITouchEnded& evt){
	if(evt.beganNode != mNode)
		return;
	Vector3f q;
	testExtra(evt.tpos, q, Vector2f(0.f,0.f));
	Rectanglef const& frame( ((PartialSprite*)mNode->getSprite())->getOriginFrame() );
	setPercent((q[0]-frame.Origin[0])/frame.Extent[0]);
	sendMessage(MSG_CONVERT(float,mPercent));
	evt.processed();
}
