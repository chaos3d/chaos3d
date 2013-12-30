#include "re/gles20/render_device.h"

namespace gles20 {
    gl_texture* render_device::create_texture() {
        return new gl_texture();
    }

    gl_vertex_buffer* render_device::create_buffer(size_t size, int type) {
        return new gl_vertex_buffer(size, type);
    }
    
    gl_vertex_index_buffer* render_device::create_index_buffer(size_t size, int type) {
        return new gl_vertex_index_buffer(size, type);
    }

    gl_gpu_program* render_device::create_program() {
        return new gl_gpu_program();
    }
    
    gl_gpu_shader* render_device::create_shader(int type) {
        return new gl_gpu_shader(type);
    }
}