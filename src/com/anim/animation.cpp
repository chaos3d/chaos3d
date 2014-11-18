#include "com/anim/animation.h"
#include "action_support/action_animation.h"
#include "common/log.h"

using namespace com;

animation::animation(game_object* go)
: component(go){
    
}

::action::ptr animation::make_action(const std::string &clip_name) const {
    auto it = _clips.find(clip_name);
    if (it == _clips.end()) {
        LOG_WARN("animation clip couldn't be found: " << clip_name);
        return nullptr;
    }
    
    auto* anim = new act::action_animation(it->second, this);
    return ::action::ptr(anim);
}