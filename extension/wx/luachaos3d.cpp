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

IMPORT_CLASS_TYPE(render_device);

static int c3d_lua_create_wxwindow(lua_State* L) {
    auto checker = &wxLuaState::wxlua_Error;
    if (checker == nullptr) {
        luaL_error(L, "wx lib isn't being loaded.");
        return 0;
    }

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
                                            nullptr, // FIXME
                                            render_target::target_size_t(dim[2], dim[3]),
                                            render_window::window_pos_t(dim[0], dim[1]));
    converter<gles20::render_wxwindow_egl*>::to(L, window);
    window->release();
    return 1;
}

static void init_wx() {
    
}

extern "C" int luaopen_chaos3d_wx(lua_State *L) {
    class_<render_device>::type()
    .def("create_wxwindow", c3d_lua_create_wxwindow)
    ;
    
    auto state = state::create(L);
    state->import("chaos3d")
    .def("init", LUA_BIND(&init_wx))
    ;

    lua_getglobal(L, "chaos3d");
   
    return 1;
}
