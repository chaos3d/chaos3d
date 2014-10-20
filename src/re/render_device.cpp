#include "render_device.h"
#include "gles20/render_gles20.h"
#include "common/log.h"
#include <cassert>

DEFINE_LOGGER(render_device, "render");
DEFINE_LOGGER(gpu_shader, "render.gpu");

render_device* render_device::get_device(uint8_t type) {
    static render_device* _one_device_for_now = nullptr;
    
    assert(_one_device_for_now == nullptr || type == None);
    if(_one_device_for_now != nullptr)
        return _one_device_for_now;
    
    switch (type) {
        case OpenGLES20:
            _one_device_for_now = gles20::create_device();
            break;
        case None:
        default:
            break;
    }
    
    // TODO: device not available
    //assert(_one_device_for_now != nullptr);
    return _one_device_for_now;
};

template<> void render_device::release<render_context>(render_context* obj) {
    release_context(obj);
}

void render_device::release_context(render_context* context) {
    delete context; // TODO: don't release but save it for recycling
}
