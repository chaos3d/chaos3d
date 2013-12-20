#include "re/gles20/render_device.h"

namespace gles20 {
    gl_texture* render_device::create_texture() {
        return new gl_texture();
    }

    gl_vertex_buffer* render_device::create_buffer(size_t size, int type) {
        return new gl_vertex_buffer(size, type);
    }
    
    gl_vertex_index_buffer* render_device::create_index_buffer(size_t size) {
        assert(0);
        return nullptr;
    }

}