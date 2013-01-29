#include "2d/Render2DManager.h"
#include "2d/HardwareBuffer.h"
#include "script/bind.h"

//using namespace chaos;

IMPLEMENT_CLASS(Render2DManager, LIB_DOMAIN )
IMPLEMENT_FUNC(createWindow, &Render2DManager::createWindow)
IMPLEMENT_END;

Render2DManager::Render2DManager(){
}

Render2DManager::~Render2DManager(){
	LOG( "Max cache: %d, %d", mCachedVB[0].capacity(),mCachedVB[1].capacity());
}

HardwareBuffer* Render2DManager::cachedVertexBuffer(){
	if( mCachedVB[0].empty() ){
		HardwareBuffer* vb = createHardwareBuffer( Cache4Vertex );
		vb->retain();
		mCachedVB[1].push_back( vb );
	}else{
		mCachedVB[1].push_back( mCachedVB[0].back() );
		mCachedVB[0].pop_back();
	}

	return mCachedVB[1].back();
}

HardwareBuffer* Render2DManager::cachedIndexBuffer(){
	if( mCachedIB[0].empty() ){
		HardwareBuffer* ib = createHardwareBuffer( Cache4Index, 0, INDEX_BUFFER );
		ib->retain();
		mCachedIB[1].push_back( ib );
	}else{
		mCachedIB[1].push_back( mCachedIB[0].back() );
		mCachedIB[0].pop_back();
	}

	return mCachedIB[1].back();
}

void Render2DManager::endRender(){
	mCachedVB[0].insert(mCachedVB[0].end(), mCachedVB[1].begin(), mCachedVB[1].end() );
	mCachedIB[0].insert(mCachedIB[0].end(), mCachedIB[1].begin(), mCachedIB[1].end() );

	mCachedVB[1].clear();
	mCachedIB[1].clear();
}
