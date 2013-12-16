#include "re/gles20/render_device.h"

namespace gles20 {
    gl_texture* render_device::create_texture() {
        return new gl_texture();
    }
}