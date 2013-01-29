/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "io/MemoryDataStream.h"

///using namespace chaos;

MemoryDataStream::MemoryDataStream( char* address, size_t size) 
	: mAddress( address ), mEnd( address+size ), mCurrent( address )
{
}

size_t MemoryDataStream::read( void* buf, size_t bufSize ){
	size_t realSize = bufSize;
	if( mCurrent + bufSize < mEnd )
		realSize = mEnd - mCurrent;
	
	memcpy( buf, mCurrent, realSize );
	return realSize;
}

bool MemoryDataStream::end(){
	return mCurrent < mEnd;
}

bool MemoryDataStream::valid(){
	return mAddress != 0;
}

bool MemoryDataStream::seek( long offset, int pos ){
	if( pos == DataStream::Seek_Cur ){
		return mCurrent + offset < mEnd && ((mCurrent += offset), true);
	}

	else if( pos == DataStream::Seek_End ){
		return -offset < mEnd-mAddress && ((mCurrent = mEnd+offset), true);
	}

	else if( pos == DataStream::Seek_Set ){
		return mAddress + offset < mEnd && ((mCurrent = mAddress+offset), true);
	}

	return false;
}

long MemoryDataStream::tell(){
	return mCurrent - mAddress;
}

char const* MemoryDataStream::where(){
	return "memory";
}
