#include "com/anim/animation.h"

using namespace com;

animation::animation(game_object* go)
: component(go){
    
}

action::ptr animation::make_animation(const std::string &clip_name) const {
    action::ptr ptr;
    
    return ptr;
}