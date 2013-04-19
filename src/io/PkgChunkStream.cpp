/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "common/common.h"
#include "io/PkgChunkStream.h"

//using namespace chaos;

PkgChunkStream::PkgChunkStream(DataStream* ds, long offset, size_t size, char const* name)
	: mStream(0), mStartOff(offset), mSize(size), mName(name), mOffset(0L)
{
	SAFE_REFAGN(mStream,ds);
	mStream->seek(mStartOff, Seek_Set);
}

PkgChunkStream::~PkgChunkStream(){
	SAFE_RELEASE0(mStream);
}

bool PkgChunkStream::end( ){
	return mOffset >= (long)mSize;
}

bool PkgChunkStream::valid( ){
	return mStream && mStream->valid();
}

size_t PkgChunkStream::read( void* buf, size_t bufSize ){
	size_t actSize = mOffset + bufSize > mSize ? mSize-mOffset : bufSize;
	actSize = mStream->read(buf, actSize);
	mOffset += actSize;
	return actSize;
}

void PkgChunkStream::close(){
	mStream->close();
	SAFE_RELEASE0(mStream);
}

bool PkgChunkStream::seek(long offset, int pos){
	if( pos == Seek_Cur ){
		if( mOffset + offset >= 0 && mOffset + offset <= (long)mSize )
			return mStream->seek(offset, Seek_Cur) && (mOffset += offset, true);
		else
			return false;
	}else if( pos == Seek_Set ){
		if( offset >= 0 && offset <= (long)mSize )
			return mStream->seek(mStartOff + offset, Seek_Cur) && (mOffset=offset, true);
		else
			return false;
	}else if( offset < 0 && -offset <= (long)mSize ){
		return mStream->seek(mStartOff + mSize + offset, Seek_End) && (mOffset=mSize+offset, true);
	}else
		return false;
	return false;
}

long PkgChunkStream::tell(){
	return mOffset;
}
