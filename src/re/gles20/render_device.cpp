#include "re/gles20/render_device.h"

namespace gles20 {
    gl_texture* render_device::create_texture() {
        return new gl_texture();
    }

    vertex_buffer* render_device::create_buffer(std::initializer_list<vertex_buffer::channel_desc> const&,
                                                size_t size, uint8_t* data,
                                                bool client,
                                                bool interleaved) {
        assert(0);
        return nullptr;
    }
}