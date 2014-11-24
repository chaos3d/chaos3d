/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef _CHAOS3D_IO_DATA_STREAM_H
#define _CHAOS3D_IO_DATA_STREAM_H

#include <memory>

/// abstraction layer of underlying input stream
class data_stream {
public:
    typedef std::unique_ptr<data_stream> ptr;
    typedef std::unique_ptr<data_stream const> const_ptr;
    
public:
    enum{
        SeekCur,    // relative to the current position
        SeekEnd,    // relative to the end of stream
        SeekSet     // relative to the start of stream
    };
    
    data_stream(){};
    virtual ~data_stream() {};
    
    /// check if the stream is valid to read
    virtual bool valid() = 0;
    
    /// check if the read pointer reaches the end
    virtual bool end() = 0;
    
    /// read bufSize into buf
    /// @return the actual number of bytes read
    virtual size_t read(void* buf, size_t bufSize) = 0;
    
    /// move the read pointer
    /// @param offset, positive or negative
    /// @param pos, SeekCur, SeekEnd, or SeekSet (start)
    virtual bool seek(long offset, int pos = SeekCur) = 0;
    
    /// tell where the current read pointer is
    virtual long tell() = 0;
    
    /// implicitly close the stream
    virtual void close() = 0;
    
    /// the size/length of the stream
    virtual size_t size() const = 0;
    
    /// reset the stream to the initial state like it was just to be open
    virtual bool reset();
};

#endif
