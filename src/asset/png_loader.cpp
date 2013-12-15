#include "asset/png_loader.h"
#include "io/memory_stream.h"

png_loader::png_loader(data_stream* source) {
    // TODO: load png data
}

png_loader::~png_loader() {
    if(_buffer)
        delete [] _buffer;
    
    _buffer = nullptr;
}

data_stream* png_loader::data() const {
    return new memory_stream(_buffer, _buf_size);
}