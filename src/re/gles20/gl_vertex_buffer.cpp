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
    
    GLASSERT0;
    
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void gl_vertex_buffer::bind() {
    GLASSERT1(glIsBuffer(_buffer_id) == GL_TRUE);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_id);
}

void gl_vertex_buffer::unbind() {
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void* gl_vertex_buffer::lock(size_t offset, size_t size) {
#if GL_OES_mapbuffer
    assert(offset + size <= vertex_buffer::size());
    GLASSERT1(glIsBuffer(_buffer_id) == GL_TRUE);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_id);
    return (char*)glMapBufferOES(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES) + offset;
#else
    assert(0);
    return nullptr;
#endif
}

void gl_vertex_buffer::unlock() {
#if GL_OES_mapbuffer
    GLASSERT1(glIsBuffer(_buffer_id) == GL_TRUE);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_id);
    glUnmapBufferOES(_buffer_id);
#else
    assert(0);
#endif
}

void gl_vertex_buffer::load(void* data, size_t offset, size_t size) {
    GLASSERT1(glIsBuffer(_buffer_id) == GL_TRUE);
    assert(offset + size <= vertex_buffer::size());
    glBindBuffer(GL_ARRAY_BUFFER, _buffer_id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

#pragma mark - index buffer

gl_vertex_index_buffer::gl_vertex_index_buffer(size_t size, int type)
: vertex_index_buffer(size, type) {
    
	glGenBuffers( 1, &_buffer_id );
    
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _buffer_id );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, size, nullptr, _usage_map[type] );
    
    GLASSERT0;
    
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void gl_vertex_index_buffer::bind() {
    GLASSERT1(glIsBuffer(_buffer_id) == GL_TRUE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer_id);
}

void gl_vertex_index_buffer::unbind() {
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void* gl_vertex_index_buffer::lock(size_t offset, size_t size) {
#if GL_OES_mapbuffer
    assert(offset + size <= vertex_buffer::size());
    GLASSERT1(glIsBuffer(_buffer_id) == GL_TRUE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer_id);
    return (char*)glMapBufferOES(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY_OES) + offset;
#else
    assert(0);
    return nullptr;
#endif
}

void gl_vertex_index_buffer::unlock() {
#if GL_OES_mapbuffer
    GLASSERT1(glIsBuffer(_buffer_id) == GL_TRUE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer_id);
    glUnmapBufferOES(_buffer_id);
#else
    assert(0);
#endif
}

void gl_vertex_index_buffer::load(void* data, size_t offset, size_t size) {
    GLASSERT1(glIsBuffer(_buffer_id) == GL_TRUE);
    assert(offset + size <= vertex_buffer::size());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer_id);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
}
