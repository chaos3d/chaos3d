#include "com/scene2d/world_box2d.h"
#include <Box2D/Box2D.h>

using namespace scene2d;

#pragma mark - collider2d -

struct collider2d::internal{
    
};

collider2d::collider2d(game_object* go)
: component(go), _internal(new internal)
{    
}

collider2d& collider2d::operator=(collider2d const& rhs){
    return *this;
}

void collider2d::destroy() {
    delete this;
}

#pragma mark - world2d mgr -

struct world2d_mgr::internal {
    b2World world;
};

world2d_mgr::world2d_mgr(vector2f const& gravity)
: _internal(new internal{
    b2Vec2(gravity.x(), gravity.y()),
}){
    
}

void world2d_mgr::update(goes_t const&) {
    _internal->world.Step(1.f/30.f, 6, 2);
}
