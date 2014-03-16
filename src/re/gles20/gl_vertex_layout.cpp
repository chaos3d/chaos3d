#include <typeinfo>

#include "re/gles20/gl_vertex_layout.h"
#include "re/gles20/gl_vertex_buffer.h"
#include "re/render_context.h"

static GLenum _mode_map [] = {
    GL_POINTS,          // Points,
    GL_LINES,           // Lines,
    GL_TRIANGLE_STRIP,  // TriangleStrip,
    GL_TRIANGLE_FAN,    // TriangleFan,
    GL_TRIANGLES,       // Triangles;
};

static GLenum _type_map [] = {
    GL_UNSIGNED_BYTE,   // U8
    GL_FLOAT,           // Float
};

gl_vertex_layout::gl_vertex_layout(channels_t&& channels, vertex_index_buffer::ptr&& buffer, uint8_t mode)
: vertex_layout(std::move(channels), std::move(buffer), mode)
{
    build_buffers();
    create_vao();
}

gl_vertex_layout::~gl_vertex_layout() {
    delete_vao();
}

void gl_vertex_layout::delete_vao() {
    glDeleteVertexArraysOES(1, &_vao_id);
}

void gl_vertex_layout::draw(render_context *context, size_t start, size_t count) const{
    if(count == 0)
        return;
        
    context->apply();
    bind_vao();
    
    // TODO: bind client buffers
    if(index_buffer_raw()) {
        assert(typeid(*index_buffer()) == typeid(gl_vertex_index_buffer));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                     static_cast<gl_vertex_index_buffer*>(index_buffer_raw())->buffer_id());
        glDrawElements(_mode_map[mode()], (GLsizei)count, GL_UNSIGNED_SHORT, reinterpret_cast<void*>(start)); // FIXME: index type
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        GLNOERROR;
    } else {
        glDrawArrays(_mode_map[mode()], (GLint)start, (GLsizei)count);
        GLNOERROR;
    }
    
    unbind_vao();
}

void gl_vertex_layout::bind_vao() const {
    glBindVertexArrayOES(_vao_id);
    GLNOERROR;
}

void gl_vertex_layout::unbind_vao() const{
    glBindVertexArrayOES(0);
    GLNOERROR;
}

void gl_vertex_layout::create_vao() {
    glGenVertexArraysOES(1, &_vao_id);
    glBindVertexArrayOES(_vao_id);
    
    gl_vertex_buffer* cur = nullptr;
    for(auto& it : _buffers) {
        if(it.buffer == nullptr)
            continue;
        
        if(it.buffer != cur) {
            cur = it.buffer;
            glBindBuffer(GL_ARRAY_BUFFER, it.buffer->buffer_id());
        }
        auto const& channel = channels()[it.index];
        glEnableVertexAttribArray(it.index);
        glVertexAttribPointer(it.index, channel.unit, _type_map[channel.type],
                              GL_FALSE, (GLsizei)channel.stride, (void*)channel.offset);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArrayOES(0);
    GLNOERROR;
}

void gl_vertex_layout::build_buffers() {
    _buffers.reserve(channels().size());
    int idx = 0;
    for(auto& it : channels()) {
        _buffers.emplace_back(it.buffer.get(), idx++);
    }
    
    std::sort(_buffers.begin(), _buffers.end(), [=] (buffer_channel const& lhs, buffer_channel const rhs) {
        return lhs.buffer < rhs.buffer;
    });
}