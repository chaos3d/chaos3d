#include "script/type/def.h"
#include "script/class_type.h"
#include "script/lua_bind.h"
#include "script/state.h"

#include "go/game_object.h"
#include "action_support/action_transform.h"
#include <vector>
#include <array>

template class script::class_<root_action>;

namespace script {
    // TODO: converter from array to key_frame_t
    action* c3d_go_make_translate_action(game_object* go, float duration,
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
    
    action* c3d_action_add_sequence(action* act, std::vector<action*> const& seq) {
        act->push(action::sequence(seq));
        return act;
    }
    
    action* c3d_action_add_group(action* act, std::vector<action*> const& grp) {
        act->push(action::group(grp));
        return act;
    }

    void def_action(state* st, std::string const& scope) {
        st->import((scope + ".action").c_str())
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
