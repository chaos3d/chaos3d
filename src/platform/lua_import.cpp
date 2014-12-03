#include "re/render_device.h"
#include "re/native_window.h"
#include "re/gles20/render_device.h"

#include "script/type/def.h"
#include "script/class_type.h"
#include "script/lua_bind.h"
#include "script/state.h"
#include "script/lua_ref.h"
#include "script/import_scope.h"
#include "script/converter_ptr.h"

#include "go/game_object.h"
#include "sg/transform.h"
#include "com/sprite2d/sprite.h"
#include "com/sprite2d/texture_atlas.h"
#include "com/scene2d/world_box2d.h"
#include "com/scene3d/collider3d.h"
#include "com/render/camera_mgr.h"
#include "com/action/action.h"

#include "asset/asset_manager.h"
#include "asset/asset_locator.h"
#include "asset_support/texture_asset.h"

#include "common/timer.h"
#include <array>

using namespace script;

template class script::class_<native_window>;
template class script::class_<render_device>;

static bool initialize_mgr(render_device* dev, render_context* ctx) {
    component_manager::initializer(
                                   make_manager<com::transform_manager>(),
                                   make_manager<scene3d::world3d_mgr>(),
                                   make_manager<scene2d::world2d_mgr>(0.02f, Eigen::Vector2f{0.f,0.f}),
                                   make_manager<sprite2d::sprite_mgr>(dev),
                                   make_manager<com::camera_mgr>(dev, ctx),
                                   make_manager<com::action_mgr>()
                                   );

    // FIXME: contextual scaling and bundle config
    auto& asset_mgr = global_asset_mgr::create({.scale = 2.f});
    
    for (auto& it : {
        locator::dir_locator::app_dir(0, "/res"),
        locator::dir_locator::cur_dir(1, "/res"),
    }) {
        if (!it)
            continue;
        asset_mgr.add_from_bundle(png_asset_bundle::bundle(dev, it).get());
    }
    return true;
}

extern "C" void c3d_lua_import(lua_State *L) {
#if 0
    scene2d::world2d_mgr::instance()
    .set_position_iteration(3)
    .set_velocity_iteration(8);    
#endif
    
    static auto state = state::create(L);

    // to load the lib
    luaL_Reg funcs[] = {
        {"load_atlas", LUA_BIND((&texture_atlas::load_from<ref, asset_manager&>))},
        {"init_mgr", LUA_BIND(&initialize_mgr)},
        {"create_realtime_timer", LUA_BIND(&make_global_timer<timer::ticker_realtime>)},
        {"create_fixed_timer", LUA_BIND((&make_global_timer<timer::ticker_fixed, timer::frame_t>))},
        {NULL, NULL}
    };
    luaL_register(L, "chaos3d", funcs);
    
    state->import("chaos3d")
    .def_singleton_getter<sprite2d::sprite_mgr>("get_sprite_mgr")
    .def_singleton_getter<scene2d::world2d_mgr>("get_world2d_mgr")
    .def_singleton_getter<global_asset_mgr, asset_manager>("get_asset_mgr")
    .def_singleton_getter<locator_mgr>("get_locator")
    ;
    
    script::class_<native_window>::type()
    ;
    
    script::def_render_device(state.get(), "chaos3d");
    script::def_game_object(state.get(), "chaos3d");
    script::def_eigen_math();
    script::def_sprite2d(state.get(), "chaos3d");
    script::def_asset();
    script::def_action(state.get(), "chaos3d");
    script::def_event(state.get(), "chaos3d");
    script::def_stream(state.get(), "chaos3d");
}
