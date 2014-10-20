/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "io/file_stream.h"
#include "common/log.h"
#include <sys/stat.h>

INHERIT_LOGGER(file_stream, data_stream);

file_stream::file_stream( char const* filename ) : _size(0) {
    _file = fopen(filename , "rb");

    struct stat st;
    
    if (stat(filename, &st) == 0)
        _size = (size_t)st.st_size;
    
    if (_file == 0) {
        LOG_ERROR("Unable to open file stream:" << filename);
    } else {
        LOG_INFO("Open file stream:" << filename);
    }
}

file_stream::~file_stream(){
    if (_file != 0)
        fclose(_file);
}

bool file_stream::end( ){
    return feof(_file) != 0;
}

bool file_stream::valid( ){
    return _file != 0;
}

size_t file_stream::read( void* buf, size_t bufSize ){
    return fread(buf, 1, bufSize, _file);
}

bool file_stream::seek( long offset, int pos ){
    static int origin[] = {SEEK_CUR, SEEK_END, SEEK_SET};
    return fseek(_file, offset, origin[pos]) == 0;
}

long file_stream::tell(){
    return ftell(_file);
}

void file_stream::close(){
    fclose(_file);
    _file = 0;
}

size_t file_stream::size() const{
    return _size;
}

//#ifdef	WIN32
bool file_stream::exist( const char* filename ){
    FILE* f = fopen(filename, "rb");
    LOG_DEBUG(file_stream, "File (" << filename << ") exists? " << (f == 0 ? "false" : "true"));
    if (f == 0)
        return false;
    else
        return fclose(f), true;
}
//#endif
