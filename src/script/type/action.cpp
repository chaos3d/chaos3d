#include "script/type/def.h"
#include "script/class_type.h"
#include "script/lua_bind.h"
#include "script/state.h"

#include "go/game_object.h"
#include "action_support/action_transform.h"
#include "action/action_script.h"
#include "action/action_timed.h"
#include <vector>
#include <array>

template class script::class_<root_action>;

namespace script {
    // TODO: converter from array to key_frame_t
    static action* c3d_go_make_translate_action(game_object* go, float duration,
                                         std::vector<std::array<float, 4>> const&keyframe) {
        vec3f_anim_kf_t::key_frames_t key_frames;
        for (auto& it : keyframe) {
            key_frames.emplace_back(vec3f_anim_kf_t::key_frame_t(it[0],
                                                                 vec3f_anim_kf_t::key_frame_t::key_t(it[1],
                                                                                                     it[2],
                                                                                                     it[3])));
        }
        return act::make_translate_action(go, duration,
                                          vec3f_anim_kf_t::create(WRAP_CLAMP, key_frames));
    }
    
    static action* c3d_action_add_sequence(action* act, std::vector<action*> const& seq) {
        act->push(action::sequence(seq));
        return act;
    }
    
    static action* c3d_action_add_group(action* act, std::vector<action*> const& grp) {
        act->push(action::group(grp));
        return act;
    }

    static int c3d_lua_make_script_action(lua_State* L) {
        luaL_argcheck(L, lua_isfunction(L, -1), 1, "expect only one function");
        
        state* st = nullptr;
        lua_getallocf(L, (void**)&st);
        converter<action*>::to(L, new action_script(st->load()));
        return 1;
    }
    
    static int c3d_lua_make_timer_action(lua_State* L) {
        state* st = nullptr;
        lua_getallocf(L, (void**)&st);
        converter<action*>::to(L, action_timer::wait(lua_tonumber(L, 1)));
        return 1;
    }

    static int c3d_lua_make_frame_action(lua_State* L) {
        state* st = nullptr;
        lua_getallocf(L, (void**)&st);
        converter<action*>::to(L, action_frame::yield(lua_tonumber(L, 1)));
        return 1;
    }

    void def_action(state* st, std::string const& scope) {
        st->import((scope + ".action").c_str())
        .def("from", &c3d_lua_make_script_action)
        .def("wait_time", &c3d_lua_make_timer_action)
        .def("wait_frame", &c3d_lua_make_frame_action)
        ;

        script::class_<action>::type()
        .def("add_sequence", LUA_BIND(&c3d_action_add_sequence))
        .def("add_group", LUA_BIND(&c3d_action_add_group))
        ;

        script::class_<root_action>::type()
        .derive<action>()
        ;
        
        script::class_<game_object>::type()
        .def("make_translate_action", LUA_BIND(&c3d_go_make_translate_action))
        ;
    }
}
