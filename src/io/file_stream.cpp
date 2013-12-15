/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "io/FileStream.h"

//using namespace chaos;

FileStream::FileStream( char const* filename ) : mName(filename){
	mFile = fopen (filename , "rb");

	//if( mFile == 0 )
	//	LOG("Unable to open file: %s", filename);
}

FileStream::~FileStream(){
	if( mFile != 0 )
		fclose( mFile );
}

bool FileStream::end( ){
	return feof( mFile ) == 0;
}

bool FileStream::valid( ){
	return mFile != 0;
}

size_t FileStream::read( void* buf, size_t bufSize ){
	return fread( buf, 1, bufSize, mFile );
}

bool FileStream::seek( long offset, int pos ){
	static int origin[] = {SEEK_CUR, SEEK_END, SEEK_SET};
	return fseek( mFile, offset, origin[pos] ) == 0;
}

long FileStream::tell(){
	return ftell(mFile);
}

void FileStream::close(){
	fclose( mFile );
	mFile = 0;
}

//#ifdef	WIN32
bool FileStream::exist( const char* filename ){
	FILE* f = fopen( filename, "rb" );
	if( f == 0 )
		return false;
	else
		return fclose(f), true;
}
//#endif
