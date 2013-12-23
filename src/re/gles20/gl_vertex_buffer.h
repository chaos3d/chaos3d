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
    virtual void load(void*, size_t offset, size_t size) override;

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
    virtual void load(void*, size_t offset, size_t size) override;
    
private:
    GLuint _buffer_id;
};

class gl_vertex_channels : public vertex_channels {
public:
    virtual void bind() override;
    virtual void unbind() override;
};

#endif