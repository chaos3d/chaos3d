#include "re/gles20/gl_vertex_buffer.h"

static GLenum _usage_map[] = {
    GL_STATIC_DRAW,     // Static
    GL_DYNAMIC_DRAW,    // dynamic
    GL_STREAM_DRAW,     // Stream
};

#pragma mark - vertex buffer

gl_vertex_buffer::gl_vertex_buffer(size_t size, int type)
: vertex_data_buffer(size, type) {
    
	glGenBuffers( 1, &_buffer_id );
    
	glBindBuffer( GL_ARRAY_BUFFER, _buffer_id );
	glBufferData( GL_ARRAY_BUFFER, size, nullptr, _usage_map[type] );
    
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
    GLNOERROR;
}

void gl_vertex_buffer::bind() {
    GLASSERT1(glIsBuffer(_buffer_id) == GL_TRUE);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_id);
    GLNOERROR;
}

void gl_vertex_buffer::unbind() {
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
    GLNOERROR;
}

bool gl_vertex_buffer::is_locked() const {
#if GL_OES_mapbuffer
    assert(glIsBuffer(_buffer_id) == GL_TRUE);
    GLint r = GL_FALSE;
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_id);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_MAPPED_OES, &r);
    GLNOERROR;
    return r == GL_TRUE;
#else
    assert(0);
    return false;
#endif
}

void* gl_vertex_buffer::lock(size_t offset, size_t size) {
#if GL_OES_mapbuffer
    if(size == 0)
        size = vertex_buffer::size();
    assert(offset + size <= vertex_buffer::size());
    assert(glIsBuffer(_buffer_id) == GL_TRUE);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_id);
    void* buf = (char*)glMapBufferOES(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES) + offset;
    GLNOERROR;
    return buf;
#else
    assert(0);
    return nullptr;
#endif
}

void gl_vertex_buffer::unlock() {
#if GL_OES_mapbuffer
    assert(glIsBuffer(_buffer_id) == GL_TRUE);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_id);
    glUnmapBufferOES(GL_ARRAY_BUFFER);
    GLNOERROR;
#else
    assert(0);
#endif
}

void gl_vertex_buffer::load(const void* data, size_t offset, size_t size) {
    assert(glIsBuffer(_buffer_id) == GL_TRUE);
    assert(offset + size <= vertex_buffer::size());
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    GLNOERROR;
}

#pragma mark - index buffer

gl_vertex_index_buffer::gl_vertex_index_buffer(size_t size, int type)
: vertex_index_buffer(size, type) {
    
	glGenBuffers( 1, &_buffer_id );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _buffer_id );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, size, nullptr, _usage_map[type] );
    
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    GLNOERROR;
}

void gl_vertex_index_buffer::bind() {
    GLASSERT1(glIsBuffer(_buffer_id) == GL_TRUE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer_id);
    GLNOERROR;
}

void gl_vertex_index_buffer::unbind() {
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    GLNOERROR;
}

bool gl_vertex_index_buffer::is_locked() const {
#if GL_OES_mapbuffer
    assert(glIsBuffer(_buffer_id) == GL_TRUE);
    GLint r = GL_FALSE;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer_id);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_MAPPED_OES, &r);
    GLNOERROR;
    return r == GL_TRUE;
#else
    assert(0);
    return false;
#endif
}

void* gl_vertex_index_buffer::lock(size_t offset, size_t size) {
#if GL_OES_mapbuffer
    assert(offset + size <= vertex_buffer::size());
    assert(glIsBuffer(_buffer_id) == GL_TRUE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer_id);
    void* buf = (char*)glMapBufferOES(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY_OES) + offset;
    GLNOERROR;
    return buf;
#else
    assert(0);
    return nullptr;
#endif
}

void gl_vertex_index_buffer::unlock() {
#if GL_OES_mapbuffer
    assert(glIsBuffer(_buffer_id) == GL_TRUE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer_id);
    glUnmapBufferOES(GL_ELEMENT_ARRAY_BUFFER);
    GLNOERROR;
#else
    assert(0);
#endif
}

void gl_vertex_index_buffer::load(const void* data, size_t offset, size_t size) {
    assert(glIsBuffer(_buffer_id) == GL_TRUE);
    assert(offset + size <= vertex_buffer::size());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer_id);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
    GLNOERROR;
}
