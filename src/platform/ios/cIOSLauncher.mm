#include "cIOSLauncher.h"
#include "platform/launcher.h"
#include "platform/ios/cAppDelegate.h"

#include "common/timer.h"
#include "go/game_object.h"
#include "re/native_window.h"
#include "go/component_manager.h"

#include "asset/asset_locator.h"
#include "asset/asset_manager.h"

#include "script/type/def.h"
#include "script/class_type.h"
#include "script/lua_bind.h"
#include "script/state.h"
#include "script/lua_ref.h"
#include "script/import_scope.h"
#include "script/converter_ptr.h"
#include <liblua/lua/lua.hpp>

using namespace script;

#pragma mark - iOS native view

class ios_game_window : public native_window {
public:
    typedef Eigen::Vector2f vector2f;
    
public:
    ios_game_window(char const* title,
                    vector2f const& size,
                    vector2f const& origin) {
        _view = [[UIView alloc] initWithFrame: CGRectMake(origin.x(), origin.y(),
                                                          size.x(), size.y())];
    }
    
    void* handle() const override {
        return (__bridge void*) _view;
    }
    
    ~ios_game_window() {
    }
    
private:
    UIView* _view;
};

#pragma mark - iOS Launcher

class ios_launcher : public launcher {
private:
    ios_launcher() {
    }
    
    virtual bool poll_event(bool wait, timer* timer_) override {
        return true;
    }
    
    virtual native_window::ptr create_game_window(char const* title,
                                                  std::array<float, 4> const& dim) override {
        UIView* parent = [(cAppDelegate*) [UIApplication sharedApplication].delegate window].rootViewController.view;
        ios_game_window* window = new ios_game_window(title, vector2f(dim[2], dim[3]), vector2f(dim[0], dim[1]));
        [parent addSubview: (__bridge UIView*) window->handle()];
        return native_window::ptr(window);
    }
    
    friend launcher& launcher::initialize();
};

static int poll_event(lua_State* L) {
    // does nothing but yield to the main thread
    return lua_yield(L, 2);
}

vector2f launcher::get_screen() const {
    CGRect bounds = [(cAppDelegate*) [UIApplication sharedApplication].delegate window].bounds;
    return vector2f{bounds.size.width, bounds.size.height}; // FIXME: retina size?
}

char const* launcher::get_host() const {
    static dispatch_once_t onceToken;
    static std::string host(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? "ipad" : "iphone");
    dispatch_once(&onceToken, ^{
        // TODO: armv, 64bit, ios
    });
    return host.c_str();
}

launcher& launcher::initialize() {
    static ios_launcher ios;
    // TODO: move this up to common codebase
    auto* device = render_device::get_device(render_device::OpenGLES20);
    device->init_context();
    
    if (!global_timer_base::has_created()) {
        make_global_timer<timer::ticker_realtime>();
    }
   
    locator_mgr::instance().
    add(locator::dir_locator::app_dir())
    //->add(locator::dir_locator::home_dir())
    ;
    
    class_<launcher>::type()
    .def("poll_event", ::poll_event); // override this because we need to yield

    return ios;
}
