/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "io/file_stream.h"

file_stream::file_stream( char const* filename ) {
    _file = fopen (filename , "rb");

    // TODO
	//if( mFile == 0 )
	//	LOG("Unable to open file: %s", filename);
}

file_stream::~file_stream(){
    if( _file != 0 )
        fclose(_file );
}

bool file_stream::end( ){
    return feof( _file ) == 0;
}

bool file_stream::valid( ){
    return _file != 0;
}

size_t file_stream::read( void* buf, size_t bufSize ){
    return fread( buf, 1, bufSize, _file );
}

bool file_stream::seek( long offset, int pos ){
    static int origin[] = {SEEK_CUR, SEEK_END, SEEK_SET};
    return fseek( _file, offset, origin[pos] ) == 0;
}

long file_stream::tell(){
    return ftell(_file);
}

void file_stream::close(){
    fclose( _file );
    _file = 0;
}

//#ifdef	WIN32
bool file_stream::exist( const char* filename ){
    FILE* f = fopen( filename, "rb" );
    if( f == 0 )
        return false;
    else
        return fclose(f), true;
}
//#endif
