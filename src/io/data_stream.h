/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef _DATA_STREAM_H
#define _DATA_STREAM_H

class data_stream {
public:
    enum{
        SeekCur,
        SeekEnd,
        SeekSet
    };

    data_stream(){};
    virtual ~data_stream() {};

    virtual bool valid() = 0;
    virtual bool end() = 0;
    virtual size_t read(void* buf, size_t bufSize) = 0;
    virtual bool seek(long offset, int pos = SeekCur) = 0;
    virtual long tell() = 0;
    virtual void close() = 0;
};

#endif
