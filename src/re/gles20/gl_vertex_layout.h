#ifndef _GL_VERTEX_LAYOUT_H
#define _GL_VERTEX_LAYOUT_H

#include <vector>
#include "re/vertex_layout.h"
#include "re/gles20/gl_vertex_buffer.h"
#include "gles2.h"

class gl_vertex_layout : public vertex_layout {
public:
    struct buffer_channel {
        gl_vertex_buffer* buffer;
        int index;
        
        buffer_channel(vertex_buffer* buffer_, int index_)
        : buffer(static_cast<gl_vertex_buffer*>(buffer_)), index(index_)
        {
            assert(dynamic_cast<gl_vertex_buffer*>(buffer_) != nullptr);
        }
    };
    
    typedef std::vector<buffer_channel> buffer_channels_t;
    
public:
    gl_vertex_layout(channels_t const&, uint8_t mode, vertex_index_buffer*);
    virtual ~gl_vertex_layout();
    
    virtual void draw(render_context*) override;
    
protected:
    void create_vao();
    void delete_vao();
    void build_buffers();
    void bind_vao();
    
private:
    GLuint _vao_id;
    buffer_channels_t _buffers;
};

#endif