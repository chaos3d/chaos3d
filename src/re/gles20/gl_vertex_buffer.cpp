#include "re/gles20/gl_vertex_buffer.h"


gl_vertex_buffer::gl_vertex_buffer(size_t size, int type)
: vertex_buffer(size, type) {
    
}

void gl_vertex_buffer::bind() {
    
}

void gl_vertex_buffer::unbind() {
    
}

void* gl_vertex_buffer::lock(size_t offset, size_t size) {
    return nullptr;
}

void gl_vertex_buffer::unlock() {
}

void gl_vertex_buffer::load(void*, size_t offset, size_t size) {
    
}
