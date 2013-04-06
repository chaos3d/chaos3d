#include "2d/MapTiledSprite.h"
#include "script/lua.h"
#include "script/bind.h"
#include "2d/Scene2DNode.h"
#include "2d/Texture.h"
#include "2d/Canvas.h"

//using namespace chaos;

IMPLEMENT_CLASS(MapTiledSprite, LIB_DOMAIN )
IMPLEMENT_PROP(stepX, &MapTiledSprite::setStepX, &MapTiledSprite::getStepX )
IMPLEMENT_PROP(stepY, &MapTiledSprite::setStepY, &MapTiledSprite::getStepY )
IMPLEMENT_PROP(width, &MapTiledSprite::setWidth, &MapTiledSprite::getWidth )
IMPLEMENT_FUNC(setMap, &MapTiledSprite::setMap  )
IMPLEMENT_END

MapTiledSprite::MapTiledSprite(Scene2DNode *n)
: Sprite(n), mWidth(1), mStepX(1), mStepY(1){
}

void MapTiledSprite::parseElement(lua_State* L){
	Sprite::parseElement(L);
	int top = lua_gettop(L);

	lua_pushstring(L, "step");
	lua_rawget( L, top );
	if( lua_istable(L, -1 ) ){
		LUA_GET2(L);
		mStepX = (float)lua_tonumber(L,-2);
		mStepY = (float)lua_tonumber(L,-1);
	}else if( lua_isnumber(L,-1) ){
		mStepX = (float)lua_tonumber(L,-1);
		mStepY = 0;
	}else if( !lua_isnil(L,-1) )
		LOG("Unable to set step. (MapTiledSprite)");

	lua_settop(L,top);
	lua_pushstring(L, "width");
	lua_rawget( L, top );
	if( lua_isnumber(L,-1) ){
		mWidth = (int)lua_tonumber(L,-1);
	}else if( !lua_isnil(L,-1) )
		LOG("Unable to set width. (MapTiledSprite)");

	lua_settop(L,top);
	lua_pushstring(L, "tile");
	lua_rawget( L, top );
	Texture *tex(0);
	if( lua_istable(L,-1) && mCanvas != 0 && (tex = mCanvas->getTA()[0].texture) != 0){
		int i(1);
		lua_rawgeti(L, -1, 1);
		
		Vector2f revSize = tex->getSize();
		for(;!lua_isnil(L,-1); lua_rawgeti(L,-1, ++i)){
			if( lua_istable(L,-1) ){
				LUA_GET4(L);

				mElement.push_back( 
					Rectanglef(
						Vector2f((float)lua_tonumber(L,-4)/revSize[0],(float)lua_tonumber(L,-3)/revSize[1]),
						(float)lua_tonumber(L,-2)/revSize[0],(float)lua_tonumber(L,-1)/revSize[1])
					);
			}else{
				LOG("Unable to define sub-tiles. (MapTiledSprite)");
			}
			lua_settop(L, top + 1);
		}
	}

	lua_settop(L,top);
	lua_pushstring(L, "map");
	lua_rawget( L, top );
	if( lua_istable(L,-1) ){
		int i(1);
		lua_rawgeti(L, -1, 1);
		
		for(;!lua_isnil(L,-1); lua_rawgeti(L,-1, ++i)){
			mMap.push_back( (int)lua_tonumber(L,-1) );
			lua_settop(L, top + 1);
		}
	}else if( !lua_isnil(L,-1) )
		LOG("Unable to set map. (MapTiledSprite)");

	lua_settop(L,top);
	return;
}

void MapTiledSprite::setMap(TypeLua const& lua){
}


void MapTiledSprite::updateData(short startIndex, SpriteData& sd) const{
	SpriteLayout* vertex = (SpriteLayout*)(sd.vertexData = &sData.vertex);
	short* index = (short*)(sd.indexData = &sData.index);
	sd.numVertex = 0;
	sd.numIndex = 0;

	float const* color(ColorRGBA::WHITE);

	if( mNode->getColor() != 0 )
		color = mNode->getColor()->getColor();

	Rectangle3f region;
	region.Axis[0] = mRegion.Axis[0];
	region.Axis[1] = mRegion.Axis[1];
	region.Extent[0] = mRegion.Extent[0];
	region.Extent[1] = mRegion.Extent[1];

	Vector3f stepx( mStepX * region.Extent[0]*2.f*region.Axis[0]),
		stepy( mStepY * region.Extent[1]*2.f*region.Axis[1]);
	Vector3f center(mRegion.Center);

	int i = 0;
	for( TMap::const_iterator it = mMap.begin();it != mMap.end();){
		region.Center = center;
		for( int x=0; x<mWidth && it != mMap.end(); ++x, ++it, ++i){
			if( *it >= 0 && *it < (int)mElement.size()){
				Rectanglef const& bound(mElement[*it]);
				memcpy( vertex->color, color, sizeof(float)*4 );
				memcpy( vertex->texCoord, (float const*)bound.GetMPCorner(), sizeof(float)*2);
				memcpy( vertex->vtCoord, (float const*)region.GetMMCorner(), sizeof(float)*3);
				++vertex;

				memcpy( vertex->color, color, sizeof(float)*4 );
				memcpy( vertex->texCoord, (float const*)bound.GetPPCorner(), sizeof(float)*2);
				memcpy( vertex->vtCoord, (float const*)region.GetPMCorner(), sizeof(float)*3);
				++vertex;

				memcpy( vertex->color, color, sizeof(float)*4 );
				memcpy( vertex->texCoord, (float const*)bound.GetMMCorner(), sizeof(float)*2);
				memcpy( vertex->vtCoord, (float const*)region.GetMPCorner(), sizeof(float)*3);
				++vertex;

				memcpy( vertex->color, color, sizeof(float)*4 );
				memcpy( vertex->texCoord, (float const*)bound.GetPMCorner(), sizeof(float)*2);
				memcpy( vertex->vtCoord, (float const*)region.GetPPCorner(), sizeof(float)*3);
				++vertex;

				*index++ = startIndex;
				*index++ = startIndex+1;
				*index++ = startIndex+2;
				*index++ = startIndex+1;
				*index++ = startIndex+2;
				*index++ = startIndex+3;
				
				sd.numVertex+=4, sd.numIndex += 6, startIndex += 4;
			}

			region.Center += stepx;
			ASSERT( i < MAX_CACHE );
		}

		if( mStepY != 0.f )
			center += stepy;
	}
}

