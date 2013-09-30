#include "render_device.h"

#include "gles20/render_gles20.h"
#include <cassert>

render_device* render_device::get_device(uint8_t type) {
    static render_device* _one_device_for_now = nullptr;
    
    assert(_one_device_for_now == nullptr || type != None);
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
