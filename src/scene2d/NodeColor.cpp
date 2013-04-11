#include "NodeColor.h"
#include "Scene2DNode.h"

//using namespace chaos;
using namespace Wm4;

#if 0
IMPLEMENT_CLASS(NodeColor, LIB_DOMAIN )
IMPLEMENT_FUNC(set, (void (NodeColor::*) (float,float,float,float))&NodeColor::setColor)
IMPLEMENT_FUNC(linearAnimation, (&NodeColor::animation<NodeColor::ColorApplier, LinearTiming>) )
IMPLEMENT_FUNC(cubicAnimation, (&NodeColor::animation<NodeColor::ColorApplier, CubicTiming>) )
IMPLEMENT_END;

NodeColor::NodeColor(Scene2DNode* n, TypeLua const& lua) : mNode(n){
	ASSERT( n != 0 );
	lua_State *L(lua.getL());
	int top = lua_gettop(L);
	lua.get(L);

	ASSERT( lua_istable(L,-1) );
	LUA_GET4(L);
	mLocalColor[0] = (float)lua_tonumber(L,-4);
	mLocalColor[1] = (float)lua_tonumber(L,-3);
	mLocalColor[2] = (float)lua_tonumber(L,-2);
	mLocalColor[3] = (float)lua_tonumber(L,-1);

	lua_settop(L,top);
}
#endif

NodeColor::NodeColor(Scene2DNode* n) : mNode(n){
	ASSERT( n != 0 );
}

void NodeColor::updateColor(){
	mColor = mLocalColor;
	if( mNode->getParent() != 0 ){
		NodeColor* color = mNode->getParent()->getColor();
		if( color != 0 )
			 mColor *= color->getColor();
	}
}

void NodeColor::setColor(ColorRGBA const& color){
	mNode->dirtyFlag() |= Scene2DNode::D_COLOR;
	mLocalColor = color;
}

void NodeColor::setColor(float r, float g, float b, float a){
	mNode->dirtyFlag() |= Scene2DNode::D_COLOR;
	mLocalColor = ColorRGBA(r,g,b,a);
}
