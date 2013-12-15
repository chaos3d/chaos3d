/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "io/memory_stream.h"

memory_stream::memory_stream(char* address, size_t size)
: _address(address), _end(address + size), _current(address),
_owned(false)
{
}

memory_stream::memory_stream(size_t size)
: _address(nullptr), _end(nullptr), _current(nullptr),
_owned(true)
{
    _address = new char [size];
    if(!_address) {
        // TODO: logging or throw?
        return;
    }
    _end = _address + size;
    _current = _address;
}

memory_stream::~memory_stream() {
    if(_owned && _address) {
        delete [] _address;
    }
    _address = nullptr;
}

size_t memory_stream::read( void* buf, size_t bufSize ){
	size_t realSize = bufSize;
	if( _current + bufSize < _end )
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
	if( pos == data_stream::SeekCur ){
        if(_current + offset <= _end &&
           _current + offset >= _address)
            return ((_current += offset), true);
        else
            return false;
	}
	else if(pos == data_stream::SeekEnd){
		return offset <= 0 && -offset <= _end-_address &&
            ((_current = _end + offset), true);
	}
	else if( pos == data_stream::SeekSet){
		return offset >= 0 && _address + offset <= _end &&
            ((_current = _address + offset), true);
	}

	return false;
}

long memory_stream::tell(){
	return _current - _address;
}
