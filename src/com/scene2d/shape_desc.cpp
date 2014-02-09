#include "com/scene2d/shape_desc.h"
#include "com/scene2d/world_box2d.h"
#include <Box2D/Box2D.h>

using namespace scene2d;

void box::append_to(scene2d::collider2d *collider) const {
    b2Body* body = static_cast<b2Body*>(collider->internal_data());
    b2PolygonShape shape;
    shape.SetAsBox(width/2.f, height/2.f, b2Vec2((x+width)/2.f, (y+height)/2.f), angle);
    
    b2FixtureDef def;
    def.shape = &shape;
    def.userData = collider;
    def.density = 1.f;
    // FIXME: fill others
    
    body->CreateFixture(&def);
}