
#include "2d/PartialCircleSprite.h"
#include "script/lua.h"
#include "script/bind.h"
#include "2d/Scene2DNode.h"

//using namespace chaos;

PartialCircleSprite::PartialCircleSprite(Scene2DNode *n)
: PartialSprite(n){
}

void PartialCircleSprite::setPercent(float percent){
	if( percent < 0.f )
		percent = 0.f;
	else if( percent > 1.f )
		percent = 1.f;

	mPercent = percent;
	
}

void PartialCircleSprite::updateData(short startIndex, SpriteData& sd) const{
	SpriteLayout* vertex = (SpriteLayout*)(sd.vertexData = &sData.vertex);
	short* index = (short*)(sd.indexData = &sData.index);
	sd.numVertex = 0;
	sd.numIndex = 0;

	float const* color(ColorRGBA::WHITE);

	if( mNode->getColor() != 0 )
		color = mNode->getColor()->getColor();

	float const* region[] = {mRegion.GetMMCorner(),
		mRegion.GetPMCorner(),mRegion.GetMPCorner(),mRegion.GetPPCorner()};
	float const* bound[] = {mBound.GetMPCorner(),
		mBound.GetPPCorner(),mBound.GetMMCorner(),mBound.GetPMCorner()};

	if( mPercent >= 1.f ){
		memcpy( vertex->color, color, sizeof(float)*4 );
		memcpy( vertex->texCoord, bound[0], sizeof(float)*2);
		memcpy( vertex->vtCoord, region[0], sizeof(float)*3);
		++vertex;

		memcpy( vertex->color, color, sizeof(float)*4 );
		memcpy( vertex->texCoord, bound[1], sizeof(float)*2);
		memcpy( vertex->vtCoord, region[1], sizeof(float)*3);
		++vertex;

		memcpy( vertex->color, color, sizeof(float)*4 );
		memcpy( vertex->texCoord, bound[2], sizeof(float)*2);
		memcpy( vertex->vtCoord, region[2], sizeof(float)*3);
		++vertex;

		memcpy( vertex->color, color, sizeof(float)*4 );
		memcpy( vertex->texCoord, bound[3], sizeof(float)*2);
		memcpy( vertex->vtCoord, region[3], sizeof(float)*3);
		++vertex;

		*index++ = startIndex;
		*index++ = startIndex+1;
		*index++ = startIndex+2;
		*index++ = startIndex+1;
		*index++ = startIndex+2;
		*index++ = startIndex+3;
		sd.numVertex = 4;
		sd.numIndex = 6;
		return;
	}


	float percent(mPercent), rest(0.f);
	int bx(0), by(0);

	memcpy( vertex->color, color, sizeof(float)*4 );
	vertex->texCoord[0] = (bound[0][0] + bound[3][0])/2.f;
	vertex->texCoord[1] = (bound[0][1] + bound[3][1])/2.f;
	memcpy( vertex->vtCoord, (float const*)mRegion.Center, sizeof(float)*3);
	++vertex;

	memcpy( vertex->color, color, sizeof(float)*4 );
	vertex->texCoord[0] = (bound[2][0] + bound[3][0])/2.f;
	vertex->texCoord[1] = (bound[2][1] + bound[3][1])/2.f;
	vertex->vtCoord[0] = (region[2][0] + region[3][0])/2.f;
	vertex->vtCoord[1] = (region[2][1] + region[3][1])/2.f;
	vertex->vtCoord[2] = (region[2][2] + region[3][2])/2.f;
	++vertex;

	*index++ = startIndex;
	*index++ = startIndex+1;
	*index++ = startIndex+2;
	sd.numVertex += 3;
	sd.numIndex += 3;

	if( percent < .125f ){
		rest = .125f - percent;
		bx = 2; by = 3;
		goto render_rest;
	}
	
	memcpy( vertex->color, color, sizeof(float)*4 );
	memcpy( vertex->texCoord, bound[3], sizeof(float)*2);
	memcpy( vertex->vtCoord, region[3], sizeof(float)*3);
	++vertex;

	*index++ = startIndex;
	*index++ = startIndex+2;
	*index++ = startIndex+3;
	++sd.numVertex;
	sd.numIndex += 3;

	if( percent <= .375f ){
		rest = percent - .125f;
		bx = 1, by = 3;
		goto render_rest;
	}
	
	memcpy( vertex->color, color, sizeof(float)*4 );
	memcpy( vertex->texCoord, bound[1], sizeof(float)*2);
	memcpy( vertex->vtCoord, region[1], sizeof(float)*3);
	++vertex;

	*index++ = startIndex;
	*index++ = startIndex+3;
	*index++ = startIndex+4;
	++sd.numVertex;
	sd.numIndex += 3;

	if( percent <= .625f ){
		rest = percent - .375f;
		bx = 0, by = 1;
		goto render_rest;
	}
	
	memcpy( vertex->color, color, sizeof(float)*4 );
	memcpy( vertex->texCoord, bound[0], sizeof(float)*2);
	memcpy( vertex->vtCoord, region[0], sizeof(float)*3);
	++vertex;

	*index++ = startIndex;
	*index++ = startIndex+4;
	*index++ = startIndex+5;
	++sd.numVertex;
	sd.numIndex += 3;

	if( percent <= .875f ){
		rest = percent - .625f;
		bx = 2, by = 0;
		goto render_rest;
	}
	
	memcpy( vertex->color, color, sizeof(float)*4 );
	memcpy( vertex->texCoord, bound[2], sizeof(float)*2);
	memcpy( vertex->vtCoord, region[2], sizeof(float)*3);
	++vertex;

	*index++ = startIndex;
	*index++ = startIndex+5;
	*index++ = startIndex+6;
	++sd.numVertex;
	sd.numIndex += 3;

	bx = 3, by = 2;
	rest = percent - 0.875f;
	
render_rest:
	float lamdaX = rest/.25f;
	float lamdaY = 1 - lamdaX;
	memcpy( vertex->color, color, sizeof(float)*4 );
	vertex->texCoord[0] = bound[bx][0]*lamdaX + bound[by][0]*lamdaY;
	vertex->texCoord[1] = bound[bx][1]*lamdaX + bound[by][1]*lamdaY;
	vertex->vtCoord[0] = region[bx][0]*lamdaX + region[by][0]*lamdaY;
	vertex->vtCoord[1] = region[bx][1]*lamdaX + region[by][1]*lamdaY;
	vertex->vtCoord[2] = region[bx][2]*lamdaX + region[by][2]*lamdaY;

}

