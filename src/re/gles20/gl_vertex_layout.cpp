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

gl_vertex_layout::gl_vertex_layout(channels_t const& channels, uint8_t mode, vertex_index_buffer* buffer)
: vertex_layout(channels, mode, buffer)
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

void gl_vertex_layout::draw(render_context *context) {
    context->apply();
    bind_vao();
    
    if(index_buffer()) {
        assert(typeid(*index_buffer()) == typeid(gl_vertex_index_buffer));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                     static_cast<gl_vertex_index_buffer*>(index_buffer())->buffer_id());
        glDrawElements(_mode_map[mode()], size(), GL_UNSIGNED_SHORT, NULL); // FIXME: index type
    } else {
        glDrawArrays(_mode_map[mode()], 0, size());
    }
    
    assert(glGetError() == GL_NO_ERROR);
}

void gl_vertex_layout::bind_vao() {
    glBindVertexArrayOES(_vao_id);
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
                              GL_FALSE, channel.stride, (void*)channel.offset);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArrayOES(0);
}

void gl_vertex_layout::build_buffers() {
    _buffers.reserve(channels().size());
    int idx = 0;
    for(auto& it : channels()) {
        _buffers.emplace_back(it.buffer, idx++);
    }
    
    std::sort(_buffers.begin(), _buffers.end(), [=] (buffer_channel const& lhs, buffer_channel const rhs) {
        return lhs.buffer < rhs.buffer;
    });
}