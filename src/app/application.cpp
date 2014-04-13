#include "app/application.h"
#include "app/screen.h"
#include "asset/asset_locator.h"
#include "re/render_device.h"

screen& application::get_screen() const {
    static screen _default;
    return _default;
}

bool application::on_initialize() {
    _main_device = render_device::get_device(render_device::OpenGLES20);
    _main_device->init_context();
    
    _main_window = _main_device->create_window(render_target::target_size_t(320, 568));
    
    // init the context for the current thread
    _main_context = _main_device->create_context(_main_window);
    _main_context->set_current();
    
    locator_mgr::instance().
    add(locator::dir_locator::app_dir())->
    add(locator::dir_locator::home_dir());
    return true;
}