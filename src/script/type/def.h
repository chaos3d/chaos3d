#ifndef _CHAOS3D_SCRIPT_TYPE_DEF_H
#define _CHAOS3D_SCRIPT_TYPE_DEF_H

namespace script {
    class state;
    
    // define/import type info symbols
    void def_game_object();     // game_object/transform
    void def_eigen_math();      // vector/matrix/etc
    void def_asset();           // asset mgr/locators

    void def_render_device(state*, std::string const& = "");        // render_device/gpu_program/etc..
    void def_sprite2d(state* st, std::string const& scope = "");    // sprite_mgr/quad_sprite
}
#endif