#ifndef _VERTEX_BUFFER_H
#define _VERTEX_BUFFER_H

#include <vector>
#include <unordered_map>

class vertex_buffer {
public:
    enum { Static, Dynamic };

public:
    vertex_buffer(size_t size, int type);
    virtual ~vertex_buffer() {};
    
    virtual void bind() = 0;
    virtual void unbind() = 0;
    
    virtual void* lock(size_t offset, size_t size) = 0;
    virtual void unlock() = 0;
    virtual void load(void*, size_t offset, size_t size) = 0;
    
    size_t size() const { return _size; }
    int type() const { return _type; }
    
private:
    size_t _size;
    int _type;
};

// a single buffer that saves multi-channel data
// which can be interleaved
class vertex_data_buffer : public vertex_buffer {
public:
    vertex_data_buffer(size_t size, int type);
    virtual ~vertex_data_buffer() {};
    
};

class vertex_index_buffer : public vertex_buffer {
public:
    vertex_index_buffer(size_t size, int type);
    virtual ~vertex_index_buffer() {};
};

// TODO
// an array of vertex buffers
//  mapping channels to buffers
class vertex_channels {
public:
    enum { U8, Float };
    struct channel_desc {
        int type;   // unsigned/float
        int unit;   // count
        size_t offset;
        std::string name;
    };
    
    typedef std::vector<channel_desc> channels_t;
    
    typedef vertex_buffer* vertex_buffer_ptr;
    //typedef std::vector<vertex_buffer_ptr> buffers_t;
    typedef std::unordered_map<vertex_buffer_ptr, channels_t> buffers_t;
public:
    vertex_channels(std::initializer_list<vertex_buffer_ptr> buffers);

    virtual void bind() = 0;
    virtual void unbind() = 0;
    
    buffers_t const& buffers() const { return _buffers; }
    
    void set_channels(std::initializer_list<channel_desc> desc);
    channels_t const& channels() const { return _channels; }
    
private:
    channels_t _channels;
    buffers_t _buffers;
};
#endif