#ifndef _VERTEX_BUFFER_H
#define _VERTEX_BUFFER_H

#include <vector>
#include <unordered_map>

class gpu_program;

class vertex_buffer {
public:
    enum { Static, Dynamic, Stream };

public:
    vertex_buffer(size_t size, int type);
    virtual ~vertex_buffer() {};
    
    virtual void bind() = 0;
    virtual void unbind() = 0;
    
    virtual void* lock(size_t offset, size_t size) = 0; //TODO: async locking
    virtual void unlock() = 0;
    virtual void load(const void*, size_t offset, size_t size) = 0;
    
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

#endif