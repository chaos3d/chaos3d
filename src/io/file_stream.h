/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_FILE_STREAM_H
#define	_FILE_STREAM_H

#include "io/data_stream.h"
#include <cstdio>

class file_stream : public data_stream{
public:
    ~file_stream();
    file_stream(char const* filename);

    static bool exist(const char* filename);

    virtual bool valid();
    virtual void close();
    virtual bool end();
    virtual size_t read(void* buf, size_t bufSize);
    virtual bool seek(long offset, int pos);
    virtual long tell();

private:
    FILE *_file;
};

#endif

