#ifndef _GLES2_VERTEX_BUFFER_H
#define _GLES2_VERTEX_BUFFER_H

#include "re/vertex_buffer.h"
#include "re/gles20/gles2.h"

class gl_vertex_buffer : public vertex_data_buffer {
public:
    gl_vertex_buffer(size_t size, int type);
    
    virtual void bind() override;
    virtual void unbind() override;
    
    virtual void* lock(size_t offset, size_t size) override;
    virtual void unlock() override;
    virtual bool is_locked() const override;
    virtual void load(const void*, size_t offset, size_t size) override;

    GLuint buffer_id() const { return _buffer_id; }
    
private:
    GLuint _buffer_id;
};

class gl_vertex_index_buffer : public vertex_index_buffer {
public:
    gl_vertex_index_buffer(size_t size, int type);
    
    virtual void bind() override;
    virtual void unbind() override;
    
    virtual void* lock(size_t offset, size_t size) override;
    virtual void unlock() override;
    virtual bool is_locked() const override;
    virtual void load(const void*, size_t offset, size_t size) override;
    
    GLuint buffer_id() const { return _buffer_id; }
    
private:
    GLuint _buffer_id;
};

#endif