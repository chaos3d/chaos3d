#include "2d/PSSprite.h"
#include "2d/Scene2DNode.h"
#include "2d/PSCanvas.h"
#include "ps/ps.h"
#include "script/lua.h"
#include "script/bind.h"
#include "script/scripting.h"

//using namespace chaos;

IMPLEMENT_CLASS(PSSprite, LIB_DOMAIN )
IMPLEMENT_PROP(rate, &PSSprite::setRate, &PSSprite::getRate )
IMPLEMENT_END;

PSSprite::PSSprite(Scene2DNode *node)
	: Sprite(node), mParticle(0), mState(0),
	mCapacity(0), mCount(0), mRate(1.f/30.f /* TODO */), mCurrent(0.f)
{
}

void PSSprite::setCapacity(int c){
	if( c <= 0 ) c = 1;
	if( mParticle != 0 )
		delete [] mParticle;
	if( mState != 0 )
		delete [] mState;
	mCapacity = 0;

	mParticle = new Particle[c];
	if( mParticle == 0 )
		return;

	mState = new State[c];
	if( mState == 0 ){
		delete []mParticle;
		return;
	};

	mCapacity = c;
}

void PSSprite::setRate(float r) {
	mRate = r/30.f; // TODO
};

int PSSprite::emit(int count, int c){
	mCount = count;
	if( mCount + c >= mCapacity )
		c = mCapacity - mCount;

	if( c > 0 ){
		State *s = mState + mCount;
		//Vector2f pos(mNode->getTransform()->getMatrix()(0,3),
		//	mNode->getTransform()->getMatrix()(1,3));
		for( int i(0); i<c; ++i, ++s ){
			s->node = mNode;
		}
	}
	return mCount += c;
};


void PSSprite::parseElement(lua_State* L){
	Sprite::parseElement(L);
	int top = lua_gettop(L);

	lua_pushstring(L, "capacity");
	lua_rawget( L, top );
	if( lua_isnumber(L,-1) ){
		setCapacity( (int)lua_tonumber(L,-1) );
	}else{
		setCapacity( DefaultCapacity );
	}

	lua_pushstring(L, "rate");
	lua_rawget( L, top );
	if( lua_isnumber(L,-1) ){
		setRate( (float)lua_tonumber(L,-1) );
	}

	lua_settop(L,top);
}


void PSSprite::updateData(short startIndex, SpriteData& sd) const{
	sd.vertexData = mParticle;
	sd.indexData = 0;
	sd.numIndex = 0;
	sd.numVertex = mCount;
}
