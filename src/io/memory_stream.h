/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_MEMORY_STREAM_H
#define	_MEMORY_STREAM_H

#include "io/data_stream.h"


/**
 * data stream wrapper for memory
 *  creating from existing memory won't transfer ownership
 *  creating a new memory block will maintain its ownership
 */
class memory_stream : public data_stream {
public:
    typedef std::unique_ptr<memory_stream> ptr;
    
public:
    // read the data into the memory
    memory_stream(data_stream*);
    
    // create an empty memory stream/buffer
    memory_stream(size_t size);
    
    // create/copy from the memory
    memory_stream(const char* address, size_t size);
    
    // transfer the memory to be managed as a stream
    memory_stream(char* address, size_t size, bool copy = false);
    
    virtual ~memory_stream();

    virtual bool valid() override;
    virtual bool end() override;
    virtual size_t read(void* buf, size_t bufSize) override;
    virtual bool seek(long offset, int pos) override;
    virtual long tell() override;
    virtual void close() override {};
    virtual size_t size() const { return _end - _address; }

    char* address() const { return _address; }
    
    static ptr from(data_stream*, bool null_end = true);
private:
    char* _address;
    char* _end;
    char* _current;
    bool _owned;
};

#endif
