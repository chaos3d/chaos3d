#ifndef _VERTEX_BUFFER_H
#define _VERTEX_BUFFER_H

#include <vector>

// a single buffer that saves multi-channel data
// which can be interleaved
class vertex_buffer {
public:
    enum { Static, Dynamic };
    enum { U8, Float };
    struct channel_desc {
        int type;
        int format;
        size_t size;
        std::string name;
    };
    
    typedef std::vector<channel_desc> channels_t;
    
public:
    vertex_buffer(size_t size, int type);
    virtual ~vertex_buffer() {};
    
    virtual void bind() = 0;
    virtual void unbind() = 0;
    
    virtual void* lock(size_t offset, size_t size) = 0;
    virtual void unlock() = 0;
    virtual void load(void*, size_t offset, size_t size) = 0;
    
    void set_channels(std::initializer_list<channel_desc> desc);
    channels_t const& channels() const { return _channels; }
    size_t size() const { return _size; }
    
private:
    size_t _size;
    int _type;
    channels_t _channels;
};

class vertex_index_buffer {
public:
};

// an array of vertex buffers
class vertex_array {
public:
    typedef vertex_buffer* vertex_buffer_ptr;
    typedef std::vector<vertex_buffer_ptr> buffers_t;
    
public:
    vertex_array(std::initializer_list<vertex_buffer_ptr> buffers);

    virtual void bind() = 0;
    virtual void unbind() = 0;
    
    buffers_t const& buffers() const { return _buffers; }
    
private:
    buffers_t _buffers;
};
#endif