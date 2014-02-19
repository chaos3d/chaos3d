#ifndef _ASSET_PNG_LOADER_H
#define _ASSET_PNG_LOADER_H

#include "common/referenced_count.h"
#include <memory>

class data_stream;
class memory_stream;

// TODO: move this up
struct image_desc {
    typedef Eigen::Vector2i vector2i;
    enum { RGB565, RGBA8888, A8 }; // TODO: probably will support 4-bit channel
    
    vector2i size;
    int format;
};

// decode the png data and load the image
// into the memory buffer
class png_loader : public referenced_count{
public:
    typedef std::unique_ptr<png_loader, referenced_count::release_deleter> ptr;
    typedef std::unique_ptr<png_loader const, referenced_count::release_deleter> const_ptr;
    
public:
    // it will load the data right away
    //  may change this later...
    png_loader(data_stream* source, int format = image_desc::RGB565);
    
    ~png_loader();
    
    // create a stream wrapper
    //  the stream's ownership is transfered, but its
    //  data will only be valid if the loader is valid
    std::unique_ptr<memory_stream> data() const;
    
    image_desc const& image() const { return _desc; }
    size_t buf_size() const { return _buf_size; }
    const char* buffer() const { return _buffer; }
    
private:
    void load(data_stream&);
    
    image_desc _desc;
    char* _buffer;
    size_t _buf_size;
};

#endif