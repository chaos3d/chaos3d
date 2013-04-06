#include "2d/PartialSprite.h"
#include "script/lua.h"
#include "script/bind.h"
#include "2d/Scene2DNode.h"

//using namespace chaos;

IMPLEMENT_CLASS(PartialSprite, LIB_DOMAIN )
IMPLEMENT_PROP(vertical, &PartialSprite::setVertical, &PartialSprite::getVertical )
IMPLEMENT_PROP(percent, &PartialSprite::setPercent, &PartialSprite::getPercent )
IMPLEMENT_FUNC(reset, &PartialSprite::resetOrigin )
IMPLEMENT_END

PartialSprite::PartialSprite(Scene2DNode *n)
: Sprite(n), mVertical(false), mPercent(1.f){
}

void PartialSprite::parseElement(lua_State* L){
	Sprite::parseElement(L);

	resetOrigin();

	int top = lua_gettop(L);

	lua_pushstring(L, "vertical");
	lua_rawget(L, -2);
	if( lua_isboolean(L,-1) )
		setVertical(lua_toboolean(L,-1) == 1);

	lua_pushstring(L, "percent");
	lua_rawget(L, top);
	if( lua_isnumber(L,-1) )
		setPercent ( (float)lua_tonumber(L,-1) );
	
	lua_gettop(L);
	return ;
}


void PartialSprite::resetOrigin(){
	mOriginBound = mBound;
	mOriginFrame = mFrame;

	setPercent( mPercent );
}

void PartialSprite::setPercent(float percent){
	if( percent < 0.f )
		percent = 0.f;
	else if( percent > 1.f )
		percent = 1.f;

	mPercent = percent;
	
	if( mVertical ){
		mBound.Origin = mOriginBound.Origin;
		mBound.Extent[0] = mOriginBound.Extent[0];
		mBound.Extent[1] = mOriginBound.Extent[1] * mPercent;

		mFrame.Origin[0] = mOriginFrame.Origin[0];
		mFrame.Extent[0] = mOriginFrame.Extent[0];
		mFrame.Extent[1] = mOriginFrame.Extent[1] * mPercent;
		mFrame.Origin[1] = mOriginFrame.Origin[1] + mOriginFrame.Extent[1] - mFrame.Extent[1];
	}else {
		mBound.Origin = mOriginBound.Origin;
		mBound.Extent[0] = mOriginBound.Extent[0] * mPercent;
		mBound.Extent[1] = mOriginBound.Extent[1];

		mFrame.Origin = mOriginFrame.Origin;
		mFrame.Extent[0] = mOriginFrame.Extent[0] * mPercent;
		mFrame.Extent[1] = mOriginFrame.Extent[1];
	}

	updateRegion(mFrame);
}
