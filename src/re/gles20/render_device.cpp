#include "re/gles20/render_device.h"
#include "common/log.h"

namespace gles20 {
    
    texture::ptr render_device::create_texture(texture::vector2i const&size,
                                              texture::attribute_t const&attr) {
        return texture::ptr(new gl_texture(size, attr));
    }

    vertex_buffer::ptr render_device::create_buffer(size_t size, int type) {
        return gl_vertex_buffer::ptr(new gl_vertex_buffer(size, type));
    }
    
    vertex_index_buffer::ptr render_device::create_index_buffer(size_t size, int type) {
        return gl_vertex_index_buffer::ptr(new gl_vertex_index_buffer(size, type));
    }

    gpu_program::ptr render_device::create_program() {
        return gpu_program::ptr(new gl_gpu_program());
    }
    
    gpu_shader::ptr render_device::create_shader(int type) {
        return gpu_shader::ptr(new gl_gpu_shader(type));
    }
    
    vertex_layout::ptr render_device::create_layout(vertex_layout::channels_t&& channels,
                                                    vertex_index_buffer::ptr&&idx_buffer,
                                                    uint8_t mode) {
        return vertex_layout::ptr(new gl_vertex_layout(std::move(channels),
                                                       std::move(idx_buffer), mode));
    }

    render_device_capacity const& render_device::get_capacity() const {
        return _capacity;
    }

}