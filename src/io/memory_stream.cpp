/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "io/memory_stream.h"
#include "common/log.h"

INHERIT_LOGGER(memory_stream, data_stream);

memory_stream::memory_stream(data_stream* ds)
: _address(nullptr), _end(nullptr), _current(nullptr),
_owned(true)
{
    _current = _address = new char [ds->size()];
    if (!_address) {
        LOG_ERROR("Unable to allocate enough memory");
        return;
    }
    LOG_DEBUG("Allocate for memory stream, " << ds->size() << " at 0x" << std::hex << this);

    ds->read(_address, ds->size());
    LOG_DEBUG("Initiazlie data from another stream, " << std::hex << ds);
    _end = _address + ds->size();
}

memory_stream::memory_stream(const char* address, size_t size)
: memory_stream(const_cast<char*>(address), // removing const won't hurt here...
                size, true)
{
}

memory_stream::memory_stream(char* address, size_t size, bool copy)
: _address(address), _end(address + size), _current(address),
_owned(copy)
{
    if (copy) {
        _address = new char [size];
        if (!_address) {
            LOG_ERROR("Unable to allocate enough memory");
            return;
        }
        LOG_DEBUG("Allocate for memory stream, " << size << " at 0x" << std::hex << this);
        memcpy(_address, address, size);
        LOG_DEBUG("Initiazlie data from another memory, " << std::hex << address);
    } else {
        LOG_DEBUG("Inialize memory stream with unmanaged memory at 0x" << std::hex << this
                  << "from: 0x" << address);
    }
}

memory_stream::memory_stream(size_t size)
: _address(nullptr), _end(nullptr), _current(nullptr),
_owned(true)
{
    _address = new char [size];
    if (!_address) {
        LOG_ERROR("Unable to allocate enough memory");
        return;
    }
    LOG_DEBUG("Allocate for memory stream, " << size << " at 0x" << std::hex << this);
    _end = _address + size;
    _current = _address;
}

memory_stream::~memory_stream() {
    if (_owned && _address) {
        delete [] _address;
    }
    LOG_DEBUG("Deallocate memory stream? " << (_owned ? "true" : "false")
              << " at 0x" << std::hex << this);
    _address = nullptr;
}

size_t memory_stream::read(void* buf, size_t bufSize){
    size_t realSize = bufSize;
    if (_current + bufSize > _end)
        realSize = _end - _current;
    
    memcpy(buf, _current, realSize);
    _current += realSize;
    return realSize;
}

bool memory_stream::end(){
    return _current < _end;
}

bool memory_stream::valid(){
    return _address != 0;
}

bool memory_stream::seek( long offset, int pos ){
    if (pos == data_stream::SeekCur ){
        if (_current + offset <= _end &&
            _current + offset >= _address)
            return ((_current += offset), true);
        else
            return false;
    }
    else if (pos == data_stream::SeekEnd){
        return offset <= 0 && -offset <= _end-_address &&
        ((_current = _end + offset), true);
    }
    else if (pos == data_stream::SeekSet){
        return offset >= 0 && _address + offset <= _end &&
        ((_current = _address + offset), true);
    }
    
    return false;
}

long memory_stream::tell(){
    return _current - _address;
}

memory_stream::ptr memory_stream::from(data_stream* ds, bool null_end) {
    size_t size = ds->size() + (null_end ? 1 : 0);
    char* buffer = new char[size];
    
    ds->read(buffer, size);
    
    if (null_end) {
        buffer[size - 1] = '\0';
    }
    
    return ptr(new memory_stream(buffer, size));
}
