#include "2d/UIRating.h"
#include "script/lua.h"
#include "script/bind.h"
#include "2d/Scene2DNode.h"
#include "2d/MapTiledSprite.h"
#include "2d/NodeFrame.h"

//using namespace chaos;
using namespace ui;

IMPLEMENT_CLASS(UIRating, LIB_DOMAIN)
IMPLEMENT_PROP(max, &UIRating::setMax, &UIRating::getMax)
IMPLEMENT_PROP(current, &UIRating::setCurrent, &UIRating::getCurrent)
IMPLEMENT_END

UIRating::UIRating(Scene2DNode* n) : NodeUI(n),
	mCurrent(1), mMax(5), mOverlapper(true)
{
	// bug
	// check if sprite is derived from maptiledsprite
	ASSERT( mNode != 0 && mNode->getSprite() != 0);
	mEvtHandler->bind(EVT_TYPE(EventUITouchBegan), EVT_HANDLER(this,&UIRating::touchBegan));
	mEvtHandler->bind(EVT_TYPE(EventUITouchMoved), EVT_HANDLER(this,&UIRating::touchMoved));
	mEvtHandler->bind(EVT_TYPE(EventUITouchEnded), EVT_HANDLER(this,&UIRating::touchEnded));
}

void UIRating::parseElement(lua_State* L){
	NodeUI::parseElement(L);

	// bug
	// check if sprite is derived from maptiledsprite
	if( mNode->getSprite() == 0 )
		return;

	int top = lua_gettop(L);

	lua_pushstring(L, "max");
	lua_rawget(L, top);
	if( lua_isnumber(L,-1) )
		setMax((int)lua_tonumber(L,-1));

	lua_pushstring(L, "current");
	lua_rawget(L, top);
	if( lua_isnumber(L,-1) )
		setCurrent ( (int)lua_tonumber(L,-1) );
	
	lua_pushstring(L, "overlapped");
	lua_rawget(L, top);
	if( lua_isboolean(L,-1) )
		setOverlapped ( lua_toboolean(L,-1) == 1 );
	
	lua_settop(L,top);

	if( mNode->getFrame() == 0 ){
		NodeFrame *frame = new NodeFrame( mNode );
		MapTiledSprite *spr((MapTiledSprite*)mNode->getSprite());
		Rectanglef const& rt = spr->getFrame();
		float width(rt.Extent[0]*spr->getStepX()*(mMax-1));
		width += (spr->getStepX() > 0.f ? rt.Extent[0] : -rt.Extent[0]);
		width /= 2.f;
		
		frame->setFrame(
			rt.Origin[0]+width, rt.Origin[1]+rt.Extent[1]/2.f,
			width, rt.Extent[1]/2.f);
		mNode->setFrame( frame );
	}
	return ;
};

void UIRating::updateMap(){
	MapTiledSprite::TMap &map(((MapTiledSprite*)mNode->getSprite())->getMap());
	if( mOverlapper ){
		map.assign( mMax, 0 );
		map.insert( map.end(), mCurrent, 1 );
	}else{
		map.assign( mCurrent, -1 );
		map.insert( map.end(), mMax - mCurrent, 0 );
		map.insert( map.end(), mCurrent, 1 );
	}
}

void UIRating::setMax( int max ){
	MapTiledSprite *spr((MapTiledSprite*)mNode->getSprite());

	spr->getMap().reserve( max* 2);
	spr->setWidth( max );
	
	mMax = max;
	if( mCurrent > mMax )
		mCurrent = mMax;
	updateMap();
}

void UIRating::setCurrent(int cur){
	if( cur > mMax )
		cur = mMax;
	else if( cur < 0)
		cur = 0;

	mCurrent = cur;
	updateMap();
}

void UIRating::setOverlapped(bool op){
	mOverlapper = op;
	updateMap();
}

void UIRating::touchBegan(EventUITouchBegan& evt){
	NodeFrame *frame = mNode->getFrame();
	float single = frame->getExtent()[0]*2.f/mMax;
	setCurrent(int((evt.pos[0]-frame->getCenter()[0]+frame->getExtent()[0]+single)/single));
	evt.processed();
}

void UIRating::touchMoved(EventUITouchMoved& evt){
	if(evt.beganNode != mNode)
		return;
	Vector3f q;
	testExtra(evt.tpos, q, Vector2f(0.f,0.f));

	NodeFrame *frame = mNode->getFrame();
	float single = frame->getExtent()[0]*2.f/mMax;
	setCurrent(int((q[0]-frame->getCenter()[0]+frame->getExtent()[0]+single)/single));
	evt.processed();
}

void UIRating::touchEnded(EventUITouchEnded& evt){
	if(evt.beganNode != mNode)
		return;
	Vector3f q;
	testExtra( evt.tpos, q, Vector2f(0.f,0.f));

	NodeFrame *frame = mNode->getFrame();
	float single = frame->getExtent()[0]*2.f/mMax;
	setCurrent(int((q[0]-frame->getCenter()[0]+frame->getExtent()[0]+single)/single));

	sendMessage(MSG_CONVERT(float,mCurrent));
	evt.processed();
}
