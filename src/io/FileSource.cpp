/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "io/FileSource.h"
#include "script/LuaType.h"

//using namespace chaos;

/*should we make this function can open more type file, maybe ASCII? 
Not really. this implements a specific file source as a data source.
Only needs to conform to its interface.

How to manipulate its data, whether binary or text, depends on how users
use data source. As it is said, program to interface.
*/

FileSource::FileSource(const char *name, const char* mode){
	mFile = fopen( name, mode == 0 ? "wb" : mode );
}

FileSource::FileSource(const char *name){
	mFile = fopen( name, "wb" );
}

FileSource::~FileSource(){
	if( mFile != 0 )
		fclose( mFile );
}

size_t FileSource::write(void const* data, size_t size){
	return fwrite( data, 1, size, mFile );
}

bool FileSource::seek( long offset, int pos){
	static int origin[] = {SEEK_CUR, SEEK_END, SEEK_SET};
	return fseek( mFile, offset, origin[pos] ) == 0;
}

void FileSource::close(){
	fclose( mFile );
	mFile = 0;
}