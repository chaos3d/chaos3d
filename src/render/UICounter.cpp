#include "2d/UICounter.h"
#include "script/lua.h"
#include "script/bind.h"
#include "2d/Scene2DNode.h"
#include "2d/MapTiledSprite.h"
#include "2d/NodeFrame.h"
#include "anim/anim.h"

//using namespace chaos;
using namespace ui;

IMPLEMENT_CLASS(UICounter, LIB_DOMAIN)
IMPLEMENT_FUNC(animation, &UICounter::animation)
IMPLEMENT_PROP(num, &UICounter::setNum, &UICounter::getNum)
IMPLEMENT_PROP(width, &UICounter::setWidth, &UICounter::getWidth)
IMPLEMENT_PROP(base, &UICounter::setBase, &UICounter::getBase)
IMPLEMENT_PROP(trail, &UICounter::setTrail, &UICounter::getTrail)
IMPLEMENT_END

UICounter::UICounter(Scene2DNode* n) : NodeUI(n),
	mNum(0), mWidth(0), mBase(10), mTrail(-1)
{
	// bug
	// check if sprite is derived from maptiledsprite
	ASSERT( mNode != 0 && mNode->getSprite() != 0);
}

void UICounter::parseElement(lua_State* L){
	NodeUI::parseElement(L);

	// bug
	// check if sprite is derived from maptiledsprite
	if( mNode->getSprite() == 0 )
		return;

	int top = lua_gettop(L);

	lua_pushstring(L, "width");
	lua_rawget(L, -2);
	if( lua_isnumber(L,-1) )
		setWidth ( (int)lua_tonumber(L,-1) );
	
	lua_pushstring(L, "base");
	lua_rawget(L, top);
	if( lua_isnumber(L,-1) )
		setBase ( (int)lua_tonumber(L,-1) );
	
	lua_pushstring(L, "trail");
	lua_rawget(L, top);
	if( lua_isnumber(L,-1) )
		setTrail ( (int)lua_tonumber(L,-1) );
	
	lua_pushstring(L, "num");
	lua_rawget(L, top);
	if( lua_isnumber(L,-1) )
		setNum((float)lua_tonumber(L,-1));

	lua_settop(L, top);
	return ;
};

void UICounter::updateMap(){
	MapTiledSprite::TMap &map(((MapTiledSprite*)mNode->getSprite())->getMap());
	int i(0), num((int)mNum);

	map.clear();
	if( mWidth == 0){
		if( num == 0 ){
			map.push_back(0);
		}else for(int unit(num%mBase); num != 0; unit = num%mBase, ++i ){
			map.push_back(unit);
			num/=mBase;
		}
	}else for(int unit(num%mBase); i<mWidth && num != 0; unit = num%mBase, ++i ){
		map.push_back(unit);
		num/=mBase;
	}

	if( mWidth != 0 )
		map.insert( map.end(), mWidth - i, 0 );

	if( mTrail >= 0 ){
		if( map.size() < mTrail ){
			map.insert( map.end(), mTrail - map.size(), 0 );
		}
		map.insert( map.begin()+mTrail, ((MapTiledSprite*)mNode->getSprite())->getElement().size()-1);
	}
}

void UICounter::setNum( float num ){
	mNum = num;
	if( mNum < 0.f )
		mNum = 0.f;
	updateMap();
}

void UICounter::setWidth(int width){
	if( width < 0)
		return;

	mWidth = width;
	updateMap();
}

void UICounter::setBase(int base){
	if( base <= 0 )
		return;

	mBase = base;
	updateMap();
}

AnimationState*	UICounter::animation(int num){
	AnimationState* as(0);
	ValueAnimation* va = new ValueAnimation(false, &ValueAnimation::KeyFrame((float)mNum, 0.f),
		&ValueAnimation::KeyFrame((float)num, 1.f), 0);

	as = va->createState<NumberApplier,LinearTiming>(NumberApplier(this));
	va->release();
	return as;
}

