#ifndef _VERTEX_BUFFER_H
#define _VERTEX_BUFFER_H

#include "common/referenced_count.h"
#include <memory>

class gpu_program;

class vertex_buffer : public referenced_count {
public:
    enum { Static, Dynamic, Stream };
    
    typedef std::unique_ptr<vertex_buffer, referenced_count::release_deleter> ptr;
    typedef std::unique_ptr<vertex_buffer const, referenced_count::release_deleter> const_ptr;
    
public:
    vertex_buffer(size_t size, int type);
    virtual ~vertex_buffer() {};
    
    virtual void bind() = 0;
    virtual void unbind() = 0;
    
    virtual void* lock(size_t offset = 0, size_t size = 0) = 0; //TODO: async locking
    virtual void unlock() = 0;
    virtual bool is_locked() const = 0;
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
    typedef std::unique_ptr<vertex_data_buffer, referenced_count::release_deleter> ptr;
    typedef std::unique_ptr<vertex_data_buffer const, referenced_count::release_deleter> const_ptr;
    
public:
    vertex_data_buffer(size_t size, int type);
    virtual ~vertex_data_buffer() {};
};

class vertex_index_buffer : public vertex_buffer {
public:
    typedef std::unique_ptr<vertex_index_buffer, referenced_count::release_deleter> ptr;
    typedef std::unique_ptr<vertex_index_buffer const, referenced_count::release_deleter> const_ptr;
    
public:
    vertex_index_buffer(size_t size, int type);
    virtual ~vertex_index_buffer() {};
};

#endif