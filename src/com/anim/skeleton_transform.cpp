#include "com/anim/skeleton_transform.h"
#include "sg/transform.h"

using namespace com;

void skeleton_animation_clip::apply(time_t offset, transforms_t const& transforms) {
    for (auto& it : _channels) {
        auto&& key = std::get<1>(it).interpolate<void>(offset);
        auto& t =         transforms[std::get<0>(it)];
        t->set_translate(key.translate);
        t->set_rotate(key.rotation);
        t->set_scale(key.scale);
        t->set_skew(key.skew.x(), key.skew.y());
        t->mark_dirty();
    }
}
