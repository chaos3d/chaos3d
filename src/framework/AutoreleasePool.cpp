/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "core/AutoreleasePool.h"
#include "core/ReferencedCount.h"
#include "Timer.h"

//using namespace chaos;

Timer::Tick Timer::smTickPerSec = 0;
AutoreleasePool::AutoreleasePool() 
	: mCurBuffer(0), mStackPool(1){
}

AutoreleasePool::~AutoreleasePool(){
	cleanUp();
	cleanUp();
	pop( (int)mStackPool.size() );
	for( TRefList::reverse_iterator it = mGlobalPool.rbegin();
		it != mGlobalPool.rend();
		++ it )
	{
		(*it)->release();
	}
}

void AutoreleasePool::autorelease(ReferencedCount* obj, int policy){
	//todo
	switch( policy )
	{
	case AP_SINGLE:
	case AP_DOUBLE:
		mDBBufferPool[mCurBuffer].push_back( obj );
		break;
	case AP_STACKED:
		mStackPool.back().push_back( obj );
		break;
	case AP_GLOBAL:
		mGlobalPool.push_back( obj );
		break;
	}
}

void AutoreleasePool::cleanUp(){
	mCurBuffer = !mCurBuffer;
	for( TRefList::iterator it = mDBBufferPool[mCurBuffer].begin();
		it != mDBBufferPool[mCurBuffer].end();
		++ it )
	{
		(*it)->release();
	}
	mDBBufferPool[mCurBuffer].clear();
}

void AutoreleasePool::push(){
	mStackPool.push_back(TRefList());
}

void AutoreleasePool::pop( int n ){
	if( n > (int)mStackPool.size() )
		n = (int)mStackPool.size();

	int s = (int)mStackPool.size();
	for( int i = s-1; i >= s-n; -- i ){
		for( TRefList::iterator it = mStackPool[i].begin();
			it != mStackPool[i].end();
			++ it )
		{
			(*it)->release();
		}
	}
	mStackPool.resize( s-n );
	if(mStackPool.empty() )
		mStackPool.push_back(TRefList());
}
