#ifndef _VERTEX_LAYOUT_H
#define _VERTEX_LAYOUT_H

#include <vector>
#include <initializer_list>

class vertex_buffer;
class vertex_index_buffer;

// a thin wrapper that dictates how the data will be
// sent to the driver, and it will send the data
class vertex_layout {
public:
    enum { U8, Float };
    enum { Points, Lines, TriangleStrip, TriangleFan, Triangles };
    
    struct channel {
        int type;   // unsigned/float
        int unit;   // count
        size_t offset;
        vertex_buffer* buffer; // TODO: fix memory issue
    };
    
    typedef std::vector<channel> channels_t;
    
public:
    vertex_layout(std::initializer_list<channel>, uint8_t mode, vertex_index_buffer*);
    virtual ~vertex_layout() {};
    
    virtual void draw() = 0;
    
    void set_size(size_t size) { _size = size; };
    void set_mode(uint8_t mode) { _mode = mode; };
    
private:
    channels_t _channels;
    vertex_index_buffer *_index_buffer;
    uint8_t _mode;
    size_t _size;
};

#endif