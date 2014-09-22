#include "app/application.h"
#include "app/screen.h"
#include "asset/asset_locator.h"
#include "re/render_device.h"
#include "platform/device.h"

screen& application::get_screen() const {
    static screen _default;
    return _default;
}

bool application::on_initialize() {
    _main_device = render_device::get_device(render_device::OpenGLES20);
    _main_device->init_context();
    
    auto size = device::screen_size();
    _main_window = _main_device->create_window(nullptr,
                                               render_target::target_size_t(size(0), size(1)),
                                               render_window::window_pos_t(),
                                               device::best_pixel_ratio());
        
    locator_mgr::instance().
    add(locator::dir_locator::app_dir())->
    add(locator::dir_locator::home_dir());
    return true;
}