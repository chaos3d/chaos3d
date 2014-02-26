#include "com/scene2d/world_box2d.h"
#include "com/sprite2d/quad_sprite.h"
#include <Box2D/Box2D.h>

using namespace scene2d;
using namespace sprite2d;

template<>
collider2d& collider2d::reset_from<quad_sprite>(bool collidable, scene2d::shape const& def) {
    auto* quad = parent()->get_component<quad_sprite>();
    if (!quad)
        return *this;
    box box_shape(quad->bound());
    static_cast<shape&>(box_shape) = def;
    box_shape.is_collidable() = collidable;
    reset_shapes({std::move(box_shape)});
    return *this;
}
