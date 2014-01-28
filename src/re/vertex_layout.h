#ifndef _VERTEX_LAYOUT_H
#define _VERTEX_LAYOUT_H

#include "common/utility.h"
#include <vector>
#include <initializer_list>

class vertex_buffer;
class vertex_index_buffer;
class render_context;

// a thin wrapper that dictates how the data will be
// sent to the driver, and it will send the data
class vertex_layout {
public:
    enum { U8, Float, TypeMax };
    enum { Points, Lines, TriangleStrip, TriangleFan, Triangles };
    
    struct channel {
        vertex_buffer* buffer; // TODO: fix memory issue
        int type;   // unsigned/float
        int unit;   // count
        size_t offset;
        size_t stride;
        
    };
    
    typedef std::vector<channel> channels_t;
    
public:
    vertex_layout(channels_t const& channels, uint8_t mode, vertex_index_buffer* buf)
    : _channels(channels), _mode(mode), _index_buffer(buf), _size(0)
    {}
    
    virtual ~vertex_layout() {};
    
    virtual void draw(render_context*) = 0;
    
    vertex_index_buffer* index_buffer() const { return _index_buffer; }
    channels_t const& channels() const { return _channels; }

    static size_t type_size(int type) {
        assert(type >= 0 && type < TypeMax);
        static size_t _size[] = {sizeof(char), sizeof(float)};
        return _size[type];
    }
    
private:
    channels_t _channels;
    vertex_index_buffer *_index_buffer;
    
    ATTRIBUTE(uint8_t, mode);
    ATTRIBUTE(size_t, size);
};

#endif