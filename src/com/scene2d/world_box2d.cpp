#include "com/scene2d/world_box2d.h"
#include "sg/transform.h"
#include <Box2D/Box2D.h>

using namespace scene2d;

#pragma mark - internal

struct collider2d::internal{
    b2Body* body = nullptr;
    // TODO: using contact listener?
    b2Vec2 position = {0.f, 0.f};
    float z_angle = 0.f;
};

struct world2d_mgr::internal {
    b2World world;
    float piexl_ratio;
};

#pragma mark - collider2d -

collider2d::collider2d(game_object* go,
                       int collision, int type)
: component(go), _internal(new internal)
{
    b2BodyDef def;
    def.userData = this;
    def.type = collision == static_ ? b2_staticBody :
               collision == dynamic ? b2_dynamicBody : b2_kinematicBody;
    def.bullet = type == type_bullet;
    def.fixedRotation = type == type_character;
    def.active = false; // init as false, activate during first update

    _internal->body = world2d_mgr::instance()._internal->world.CreateBody(&def);
}

collider2d& collider2d::reset_shapes(std::initializer_list<shape::init_t> const& shapes,
                                     mass const& m) {
    if (shapes.size() == 0)
        return *this;
    
    clear_shapes();
    
    float ratio = world2d_mgr::instance().pixel_meter_ratio();
    for (auto& it : shapes) {
        std::get<0>(it).append_to(this, ratio);
    }
    
    if (m.weight > 0.f) {
        b2MassData data { m.weight, {m.x,m.y}, m.rotate};
        _internal->body->SetMassData(&data);
    }
    
    return *this;
}

void collider2d::clear_shapes() {
    for (auto* fixture = _internal->body->GetFixtureList(); fixture;) {
        auto* del = fixture;
        fixture = fixture->GetNext();
        _internal->body->DestroyFixture(del);
    }
}

void collider2d::update_from_transform(com::transform &transform, float ratio) {
    auto& affine = transform.global_affine();
    affine3f::LinearMatrixType rotMatrix, scaleMatrix;
    affine.computeRotationScaling(&rotMatrix, &scaleMatrix);
    auto translation = affine.translation();
    auto euler = rotMatrix.eulerAngles(0, 1, 2);
    
    if (std::fabs(euler.x() - FLT_EPSILON) > 0.f ||
        std::fabs(euler.y() - FLT_EPSILON) > 0.f ||
        std::fabs(scaleMatrix(0,0) - 1.f) > FLT_EPSILON ||
        std::fabs(scaleMatrix(1,1) - 1.f) > FLT_EPSILON ||
        std::fabs(scaleMatrix(2,2) - 1.f) > FLT_EPSILON
        ) {
        affine = Eigen::Translation3f(translation) * Eigen::AngleAxisf(euler.z(), vector3f::UnitZ());
        transform.mark_dirty(false); // update the local
    }
    
    _internal->position = {translation.x() * ratio, translation.y() * ratio};
    _internal->z_angle = euler.z();
    _internal->body->SetTransform(_internal->position, euler.z());
}

void collider2d::apply_to_transform(com::transform &transform, float ratio) const{
    // static body never changes the transform
    if (_internal->body->GetType() == b2_staticBody)
        return;
    
    auto& current = _internal->body->GetPosition();
    auto angle = _internal->body->GetAngle();
    if (current == _internal->position &&
        angle == _internal->z_angle) {
        return;
    }
    
    transform.set_global_affine(Eigen::Translation3f(current.x / ratio, current.y / ratio,
                                                     transform.global_affine().translation().z()) *
                                Eigen::AngleAxisf(angle, vector3f::UnitZ()));
    transform.mark_dirty(false);
    _internal->position = current;
    _internal->z_angle = angle;
}

void* collider2d::internal_data() const {
    return _internal->body;
}

collider2d& collider2d::operator=(collider2d const& rhs){
    return *this;
}

void collider2d::destroy() {
    _internal->body->GetWorld()->DestroyBody(_internal->body);
    delete this;
}

#pragma mark - world2d mgr -

world2d_mgr::world2d_mgr(float ratio, vector2f const& gravity)
: _internal(new internal{
    b2Vec2(gravity.x() * ratio, gravity.y() * ratio),
    ratio,
}), _velocity_iteration(6), _position_iteration(2),
_pixel_meter_ratio(ratio), _step(1.f/30.f){
    
}

void world2d_mgr::query(query_callback_t const& query,
                        vector2f const& center,
                        vector2f const& half_extent) {
    auto center_in_meter = center * pixel_meter_ratio();
    auto extent_in_meter = half_extent * pixel_meter_ratio();

    class callback : public b2QueryCallback {
    public:
        callback(query_callback_t const& cb, b2Vec2 const& center)
        : _cb(cb), _center(center) {}

    private:
        virtual bool ReportFixture(b2Fixture* fixture) {
#if 0
            if (!fixture->TestPoint(_center))
                return true;
            else
#endif
                return _cb(static_cast<collider2d*>(fixture->GetUserData()));
        }
        
        query_callback_t const& _cb;
        b2Vec2 const& _center;
    } cb(query, b2Vec2(center_in_meter.x(), center_in_meter.y()));

    _internal->world.QueryAABB(&cb,
                               b2AABB{
                                   b2Vec2(center_in_meter.x() - extent_in_meter.x(),
                                          center_in_meter.y() - extent_in_meter.y()),
                                   b2Vec2(center_in_meter.x() + extent_in_meter.x(),
                                          center_in_meter.y() + extent_in_meter.y())
                               });
}

void world2d_mgr::pre_update(goes_t const& goes) {
    auto& world = _internal->world;
    auto offset = com::transform_manager::flag_offset();
    auto transform_idx = com::transform_manager::component_idx();
    auto mark = goes.front()->mark();
    
    for (b2Body* first = world.GetBodyList(); first; first = first->GetNext()) {
        auto* collider = static_cast<collider2d*>(first->GetUserData());
        auto* obj = collider->parent();
        
        // deactivate the hidden objects
        if (obj->mark() != mark) {
            if (first->IsActive())
                first->SetActive(false);
            continue;
        }
        
        // FIXME: what if it is really deactived?
        if (!first->IsActive() ||
            (obj->flag() >> offset) & com::transform_manager::global_bit) {
            auto* transform = obj->get_component<com::transform>(transform_idx);
            if (transform) {
                collider->update_from_transform(*transform, pixel_meter_ratio());
            }
            first->SetActive(true);
            
            for (auto* contact = first->GetContactList();
                 contact != nullptr; contact = contact->next) {
                contact->other->SetAwake(true); // awake the other body
            }
        }
    }
}

void world2d_mgr::update(goes_t const&) {
    auto& world = _internal->world;
    auto transform_idx = com::transform_manager::component_idx();
    
    world.Step(_step, _velocity_iteration, _position_iteration);
    
    for (b2Body* first = world.GetBodyList(); first; first = first->GetNext()) {
        if (!first->IsAwake())
            continue;
        
        auto* collider = static_cast<collider2d*>(first->GetUserData());
        auto* obj = collider->parent();
        auto* transform = obj->get_component<com::transform>(transform_idx);
        if (transform) {
            collider->apply_to_transform(*transform, pixel_meter_ratio());
        }
    }
    
}
