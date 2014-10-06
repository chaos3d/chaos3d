#include "luachaos3d.h"
#include "render_wxwindow.h"
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

template<typename C>
static int c3d_lua_singleton_getter(lua_State *L) {
    converter<C*>::to(L, &C::instance());
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
#endif
    auto state = state::create(L);
    
    state->import("chaos3d")
    .def("load_atlas", LUA_BIND((&texture_atlas::load_from<ref, asset_manager&>)))
    .def("get_sprite_mgr", &c3d_lua_singleton_getter<sprite2d::sprite_mgr>)
    .def("get_world2d_mgr", &c3d_lua_singleton_getter<scene2d::world2d_mgr>)
    .def("get_asset_mgr", &c3d_lua_singleton_getter<global_asset_mgr>)
    .def("get_locator", &c3d_lua_singleton_getter<locator_mgr>)
    //.import("render", main_device())
    .import("root", &game_object::root())
    //.import<render_window*>("window", main_window())
    ;
    
    script::def_render_device(state.get());
    script::def_game_object(state.get());
    script::def_eigen_math();
    script::def_sprite2d(state.get());
    script::def_asset();
    script::def_event(state.get());

    lua_getglobal(L, "chaos3d");
    return 1;
}