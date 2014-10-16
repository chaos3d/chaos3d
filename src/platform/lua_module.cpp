#include "lua_module.h"

#include "re/render_device.h"
#include "re/gles20/render_device.h"

#include "script/type/def.h"
#include "script/class_type.h"
#include "script/lua_bind.h"
#include "script/state.h"
#include "script/lua_ref.h"
#include "script/import_scope.h"
#include "script/converter_ptr.h"

#include "go/game_object.h"
#include "com/sprite2d/sprite.h"
#include "com/sprite2d/texture_atlas.h"
#include "com/scene2d/world_box2d.h"

#include "asset/asset_manager.h"
#include "asset/asset_locator.h"

#include "platform/launcher.h"

#include <array>

using namespace script;

extern "C" int luaopen_chaos3d(lua_State *L) {
    auto checker = &launcher::initialize;
    if (checker == nullptr) {
        luaL_error(L, "core lib isn't being loaded.");
        return 0;
    }
    
#if 0
    set_main_context(main_device()->create_context(main_window()));
    main_context()->set_current();
    
    component_manager::initializer(
                                   make_manager<com::transform_manager>(),
                                   make_manager<scene2d::world2d_mgr>(0.02f, Eigen::Vector2f{0.f,0.f}),
                                   make_manager<sprite2d::sprite_mgr>(main_device()),
                                   make_manager<com::camera_mgr>(main_device(),
                                                                 main_context())
                                   );
    scene2d::world2d_mgr::instance()
    .set_position_iteration(3)
    .set_velocity_iteration(8);
    
    asset_manager& asset_mgr = global_asset_mgr::instance();
    // treat png as a texture
    // need a render device
    asset_mgr.add_from_bundle(png_asset_bundle::bundle(main_device(),
                                                       locator::dir_locator::app_dir()).get());
#endif
    auto state = state::create(L);
    
    state->import("chaos3d")
    //.def("init", LUA_BIND(&c3d_initialize))
    //.def("poll_event", LUA_BIND(&c3d_poll_event))
    .def("init", LUA_BIND(&launcher::initialize))
    .def("load_atlas", LUA_BIND((&texture_atlas::load_from<ref, asset_manager&>)))
    .def_singleton_getter<launcher>("get_launcher")
    .def_singleton_getter<sprite2d::sprite_mgr>("get_sprite_mgr")
    .def_singleton_getter<scene2d::world2d_mgr>("get_world2d_mgr")
    .def_singleton_getter<global_asset_mgr>("get_asset_mgr")
    .def_singleton_getter<locator_mgr>("get_locator")
    //.import("render", main_device())
    //.import<render_window*>("window", main_window())
    ;
    
    class_<launcher>()
    .def("create_game_window", LUA_BIND(&launcher::create_game_window))
    ;
    
    script::def_render_device(state.get(), "chaos3d");
    script::def_game_object(state.get(), "chaos3d");
    script::def_eigen_math();
    script::def_sprite2d(state.get(), "chaos3d");
    script::def_asset();
    script::def_event(state.get(), "chaos3d");
    
    lua_getglobal(L, "chaos3d");
    return 1;
}
