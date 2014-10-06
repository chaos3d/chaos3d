#include "luachaos3d.h"
#include "render_wxwindow.h"
#include "re/render_device.h"
#include "re/gles20/render_device.h"

#include "script/type/def.h"
#include "script/class_type.h"
#include "script/lua_bind.h"
#include "script/state.h"
#include "script/converter_ptr.h"

#include "wxlua/wxlstate.h"
#include "wxlua/wxlbind.h"
#include <wx/window.h>

#include <array>

using namespace script;

static int c3d_lua_create_wxwindow(lua_State* L) {
    wxLuaState state(L);
    
    if (!state.IsOk()) {
        // TODO log
        return 0;
    }
    
    render_device& device = converter<render_device&>::from(L, 1, nullptr);
    wxWindow* parent = (wxWindow*)state.GetUserDataType(2, *p_wxluatype_wxWindow);
    auto dim = converter<std::array<float, 4>>::from(L, 3, nullptr);

    gles20::render_wxwindow_egl* window =
        gles20::render_wxwindow_egl::create(dynamic_cast<gles20::render_device*>(&device),
                                            parent,
                                            render_target::target_size_t(dim[2], dim[3]),
                                            render_window::window_pos_t(dim[0], dim[1]));
    converter<gles20::render_wxwindow_egl*>::to(L, window);
    window->release();
    return 1;
}

extern "C" int luaopen_chaos3d_wx(lua_State *L) {
    class_<render_device>::type()
    .def("create_wxwindow", c3d_lua_create_wxwindow)
    ;
    
    return 0;
}

extern "C" int luaopen_chaos3d(lua_State *L) {
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
    
    auto state = script::state::create();
    
    state->import()
    .def("load_atlas", LUA_BIND((&texture_atlas::load_from<script::ref, asset_manager&>)))
    .import<sprite2d::sprite_mgr&>("sprite_mgr", sprite2d::sprite_mgr::instance())
    .import<scene2d::world2d_mgr&>("world2d", scene2d::world2d_mgr::instance())
    .import<asset_manager&>("asset", global_asset_mgr::instance())
    .import("render", main_device())
    .import("root", &game_object::root())
    .import<render_window*>("window", main_window())
    .import<locator_mgr&>("locator", locator_mgr::instance())
    ;
    
    script::def_render_device(state.get());
    script::def_game_object(state.get());
    script::def_eigen_math();
    script::def_sprite2d(state.get());
    script::def_asset();
    script::def_event(state.get());
#endif
    return 1;
}