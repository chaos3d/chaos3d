#include "2d/TiledSprite.h"
#include "script/lua.h"
#include "script/bind.h"
#include "2d/Scene2DNode.h"

//using namespace chaos;

IMPLEMENT_CLASS(TiledSprite, LIB_DOMAIN )
IMPLEMENT_PROP(height, &TiledSprite::setHeight, &TiledSprite::getHeight )
IMPLEMENT_PROP(width, &TiledSprite::setWidth, &TiledSprite::getWidth )
IMPLEMENT_END

TiledSprite::TiledSprite(Scene2DNode *n)
: Sprite(n), mWidth(1), mHeight(1){
}

void TiledSprite::parseElement(lua_State* L){
	int top = lua_gettop(L);

	lua_pushstring(L, "size");
	lua_rawget( L, top );
	if( lua_istable(L,-1) ){
		LUA_GET2(L);
		mWidth = (int)lua_tonumber(L,-2);
		mHeight = (int)lua_tonumber(L,-1);
	}

	lua_settop(L,top);
	return 	Sprite::parseElement(L);
}


void TiledSprite::updateData(short startIndex, SpriteData& sd) const{
	SpriteLayout* vertex = (SpriteLayout*)(sd.vertexData = &sData.vertex);
	short* index = (short*)(sd.indexData = &sData.index);
	sd.numVertex = 0;
	sd.numIndex = 0;

	float const* color(ColorRGBA::WHITE);

	if( mNode->getColor() != 0 )
		color = mNode->getColor()->getColor();

	float const*bound[] = {
		mBound.GetMPCorner(),mBound.GetPPCorner(),
		mBound.GetMMCorner(),mBound.GetPMCorner()
	};

	Rectangle3f region;
	region.Axis[0] = mRegion.Axis[0];
	region.Axis[1] = mRegion.Axis[1];
	region.Extent[0] = mRegion.Extent[0]/mWidth;
	region.Extent[1] = mRegion.Extent[1]/mHeight;

	Vector3f stepx( region.Extent[0]*2.f*region.Axis[0]),
		stepy( region.Extent[1]*2.f*region.Axis[1]);
	/*Vector3f center(mRegion.Center[0] - (mRegion.Extent[0] - mRegion.Extent[0]/mWidth),
		mRegion.Center[1] - (mRegion.Extent[1] - mRegion.Extent[1]/mHeight),
		mRegion.Center[2]);*/
	Vector3f center(mRegion.Center);
	//region.Center -= stepx*(mWidth*2.f);
	center -= ( ((mHeight-1)/2.f)*stepy );
	center -= ( ((mWidth-1)/2.f)*stepx );

	ASSERT( mWidth*mHeight < MAX_CACHE );

	for( int y=0; y<mHeight; ++y, center += stepy ){
		region.Center = center;
		for( int x=0; x<mWidth; ++x, sd.numVertex+=4, sd.numIndex += 6, startIndex += 4){
			memcpy( vertex->color, color, sizeof(float)*4 );
			memcpy( vertex->texCoord, bound[0], sizeof(float)*2);
			memcpy( vertex->vtCoord, (float const*)region.GetMMCorner(), sizeof(float)*3);
			++vertex;

			memcpy( vertex->color, color, sizeof(float)*4 );
			memcpy( vertex->texCoord, bound[1], sizeof(float)*2);
			memcpy( vertex->vtCoord, (float const*)region.GetPMCorner(), sizeof(float)*3);
			++vertex;

			memcpy( vertex->color, color, sizeof(float)*4 );
			memcpy( vertex->texCoord, bound[2], sizeof(float)*2);
			memcpy( vertex->vtCoord, (float const*)region.GetMPCorner(), sizeof(float)*3);
			++vertex;

			memcpy( vertex->color, color, sizeof(float)*4 );
			memcpy( vertex->texCoord, bound[3], sizeof(float)*2);
			memcpy( vertex->vtCoord, (float const*)region.GetPPCorner(), sizeof(float)*3);
			++vertex;

			*index++ = startIndex;
			*index++ = startIndex+1;
			*index++ = startIndex+2;
			*index++ = startIndex+1;
			*index++ = startIndex+2;
			*index++ = startIndex+3;

			region.Center += stepx;
		}
	}
}

