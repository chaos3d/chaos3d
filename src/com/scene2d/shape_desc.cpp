#include "com/scene2d/shape_desc.h"
#include "com/scene2d/world_box2d.h"
#include <Box2D/Box2D.h>

using namespace scene2d;

struct shape::shape_def {
    b2Shape* def;
};

void shape::create_from_shape(collider2d *collider, shape_def const&def) const {
    b2Body* body = static_cast<b2Body*>(collider->internal_data());
    
    b2FixtureDef fixture;
    fixture.shape = def.def;
    fixture.userData = collider;
    fixture.density = density();
    fixture.friction = friction();
    fixture.restitution = restitution();
    fixture.isSensor = is_collidable();
    fixture.filter.categoryBits = collision_category();
    fixture.filter.maskBits = collision_mask();
    fixture.filter.groupIndex = collision_group();
    
    body->CreateFixture(&fixture);
}

void box::append_to(scene2d::collider2d *collider, float ratio) const {
    b2PolygonShape shape;
    shape.SetAsBox(width/2.f*ratio, height/2.f*ratio,
                   b2Vec2((x+width/2.f)*ratio, (y+height/2.f)*ratio), angle);
    
    create_from_shape(collider, {&shape});
}

void circle::append_to(scene2d::collider2d *collider, float ratio) const {
    b2CircleShape shape;
    shape.m_p = {x*ratio, y*ratio};
    shape.m_radius = radius*ratio;

    create_from_shape(collider, {&shape});
}