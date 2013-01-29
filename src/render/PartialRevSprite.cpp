#include "2d/PartialRevSprite.h"
#include "script/lua.h"
#include "script/bind.h"
#include "2d/Scene2DNode.h"

//using namespace chaos;

PartialRevSprite::PartialRevSprite(Scene2DNode *n)
: PartialSprite(n){
}

void PartialRevSprite::setPercent(float percent){
	if( percent < 0.f )
		percent = 0.f;
	else if( percent > 1.f )
		percent = 1.f;

	mPercent = percent;
	
	if( mVertical ){
		mBound.Origin[0] = mOriginBound.Origin[0];
		mBound.Extent[0] = mOriginBound.Extent[0];
		mBound.Extent[1] = mOriginBound.Extent[1] * mPercent;
		mBound.Origin[1] = mOriginBound.Origin[1] + mOriginBound.Extent[1] - mBound.Extent[1] ;

		mFrame.Origin = mOriginFrame.Origin;
		mFrame.Extent[0] = mOriginFrame.Extent[0];
		mFrame.Extent[1] = mOriginFrame.Extent[1] * mPercent;
	}else {
		mBound.Origin[1] = mOriginBound.Origin[1];
		mBound.Extent[0] = mOriginBound.Extent[0] * mPercent;
		mBound.Extent[1] = mOriginBound.Extent[1];
		mBound.Origin[0] = mOriginBound.Origin[0] + mOriginBound.Extent[0] - mBound.Extent[0];

		mFrame.Origin[1] = mOriginFrame.Origin[1];
		mFrame.Extent[0] = mOriginFrame.Extent[0] * mPercent;
		mFrame.Extent[1] = mOriginFrame.Extent[1];
		mFrame.Origin[0] = mOriginFrame.Origin[0] + mOriginFrame.Extent[0] - mFrame.Extent[0];
	}

	updateRegion(mFrame);
}
