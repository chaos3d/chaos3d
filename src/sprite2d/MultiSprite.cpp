#include "2d/MultiSprite.h"
#include "2d/Scene2DNode.h"
#include "2d/MultiSprite.h"
#include "script/lua.h"
#include "script/bind.h"
#include "2d/Texture.h"
#include "2d/Canvas.h"

//using namespace chaos;

IMPLEMENT_CLASS(MultiSprite, LIB_DOMAIN)
IMPLEMENT_FUNC(subFrame, &MultiSprite::setSubFrame )
IMPLEMENT_FUNC(subBound, &MultiSprite::setSubBound )
IMPLEMENT_END

void MultiSprite::setSubFrame(int idx, float x,float y,float width,float height){
	ASSERT( idx >= 0 && idx < mSubSprite.size() );
	mSubSprite[idx].frame = Rectanglef( Vector2f(x,y), width, height );
}

void MultiSprite::setSubBound(int idx, float x,float y,float width,float height){
	ASSERT( idx >= 0 && idx < mSubSprite.size() );
	Texture* tex(0);
	if(mCanvas == 0 || (tex = mCanvas->getTA()[0].texture) == 0)
		return;

	Vector2f revSize = tex->getSize();
	mSubSprite[idx].bound = Rectanglef( Vector2f(x/revSize[0],y/revSize[1]), width/revSize[0], height/revSize[1] );
}

void MultiSprite::updateData(short startIndex, SpriteData& sd) const{
	if( mNode->getTransform() == 0 )
		return;

	SpriteLayout* vertex = (SpriteLayout*)(sd.vertexData = &sData.vertex);
	short* index = (short*)(sd.indexData = &sData.index);
	sd.numVertex = 4;
	sd.numIndex = 6;

	float const* color(ColorRGBA::WHITE);

	if( mNode->getColor() != 0 )
		color = mNode->getColor()->getColor();

	Matrix4f const& mat =mNode->getTransform()->getMatrix();
	Rectanglef const& bound(mBound);
	Rectangle3f const& region(mRegion);
	Vector3f unitX(mat[0][0],mat[1][0],mat[2][0]),	// x-axis
		unitY(mat[0][1],mat[1][1],mat[2][1]),		// y-axis
		origin(mat[0][3],mat[1][3],mat[2][3]);

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

	startIndex += 4;
	for( TSubSprite::const_iterator it = mSubSprite.begin();
		it != mSubSprite.end();
		++it, startIndex += 4, sd.numVertex += 4, sd.numIndex+=6 )
	{
		Rectanglef const& bound(it->bound);
		Rectangle3f frame(
			((it->frame.Origin[0] + it->frame.Extent[0]/2.f)*unitX + (it->frame.Origin[1] + it->frame.Extent[1]/2.f) * unitY + origin),
			unitX, unitY, it->frame.Extent[0]/2.f, it->frame.Extent[1]/2.f);

		memcpy( vertex->color, color, sizeof(float)*4 );
		memcpy( vertex->texCoord, (float const*)bound.GetMPCorner(), sizeof(float)*2);
		memcpy( vertex->vtCoord, (float const*)frame.GetMMCorner(), sizeof(float)*3);
		++vertex;

		memcpy( vertex->color, color, sizeof(float)*4 );
		memcpy( vertex->texCoord, (float const*)bound.GetPPCorner(), sizeof(float)*2);
		memcpy( vertex->vtCoord, (float const*)frame.GetPMCorner(), sizeof(float)*3);
		++vertex;

		memcpy( vertex->color, color, sizeof(float)*4 );
		memcpy( vertex->texCoord, (float const*)bound.GetMMCorner(), sizeof(float)*2);
		memcpy( vertex->vtCoord, (float const*)frame.GetMPCorner(), sizeof(float)*3);
		++vertex;

		memcpy( vertex->color, color, sizeof(float)*4 );
		memcpy( vertex->texCoord, (float const*)bound.GetPMCorner(), sizeof(float)*2);
		memcpy( vertex->vtCoord, (float const*)frame.GetPPCorner(), sizeof(float)*3);
		++vertex;

		*index++ = startIndex;
		*index++ = startIndex+1;
		*index++ = startIndex+2;
		*index++ = startIndex+1;
		*index++ = startIndex+2;
		*index++ = startIndex+3;
	}

}

void MultiSprite::parseElement(lua_State* L){
	Sprite::parseElement(L);

	int top = lua_gettop(L);
	Texture* tex(0);

	lua_pushstring(L, "sub");
	lua_rawget( L, top );
	if( lua_istable(L,-1) && mCanvas != 0 && (tex = mCanvas->getTA()[0].texture) != 0){
		int i(1);
		lua_rawgeti(L, -1, 1);
		
		Vector2f revSize = tex->getSize();
		for(;!lua_isnil(L,-1); lua_rawgeti(L,-1, ++i)){
			if( lua_istable(L,-1) ){
				LUA_GET4(L);

				lua_rawgeti(L, -5, 5);
				if( lua_isnil(L,-1) ){
					float bound[] = {(float)lua_tonumber(L,-5),(float)lua_tonumber(L,-4),
						(float)lua_tonumber(L,-3),(float)lua_tonumber(L,-2)};

					mSubSprite.push_back( SubSprite(
						Rectanglef(
							Vector2f(bound[0]/revSize[0],bound[1]/revSize[1]),
							bound[2]/revSize[0],bound[3]/revSize[1]
							),
						Rectanglef(
							Vector2f(-bound[2]/2.f,-bound[3]/2.f),
							bound[2],bound[3]
							) ));
				}else{
					lua_rawgeti(L, -6, 6); lua_rawgeti(L, -7, 7); lua_rawgeti(L, -8, 8);
					mSubSprite.push_back( SubSprite(
						Rectanglef(
							Vector2f((float)lua_tonumber(L,-8)/revSize[0],(float)lua_tonumber(L,-7)/revSize[1]),
							(float)lua_tonumber(L,-6)/revSize[0],(float)lua_tonumber(L,-5)/revSize[1]
							),
						Rectanglef(
							Vector2f((float)lua_tonumber(L,-4),(float)lua_tonumber(L,-3)),
							(float)lua_tonumber(L,-2),(float)lua_tonumber(L,-1)
							) ));
				}
			}
			lua_settop(L, top + 1);
		}

	}

	lua_settop(L,top);
}

