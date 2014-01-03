#include "re/gles20/render_device.h"

namespace gles20 {
    gl_texture* render_device::create_texture(texture::vector2i const&size,
                                              texture::attribute_t const&attr) {
        return new gl_texture(size, attr);
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
    
    gl_vertex_layout* render_device::create_layout(vertex_layout::channels_t const& channels,
                                                uint8_t mode, vertex_index_buffer* idx_buffer){
        return new gl_vertex_layout(channels, mode, idx_buffer);
    }

}