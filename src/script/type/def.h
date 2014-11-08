#ifndef _CHAOS3D_SCRIPT_TYPE_DEF_H
#define _CHAOS3D_SCRIPT_TYPE_DEF_H

namespace script {
    class state;
    
    // define/import type info symbols
    void def_eigen_math();      // vector/matrix/etc
    void def_asset();           // asset mgr/locators
    
    void def_action(state*, std::string const& = "");    
    void def_stream(state*, std::string const& = "");
    void def_event(state*, std::string const& = "");
    void def_game_object(state*, std::string const& = "");      // game_object/transform
    void def_render_device(state*, std::string const& = "");    // render_device/gpu_program/etc..
    void def_sprite2d(state*, std::string const& = "");         // sprite_mgr/quad_sprite
}
#endif