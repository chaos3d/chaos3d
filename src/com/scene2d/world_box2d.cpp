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

collider2d::collider2d(game_object* go, int type, int collision)
: component(go), _internal(new internal)
{
    b2BodyDef def;
    def.userData = this;
    def.type = collision == collider_static ? b2_staticBody :
    collision == collider_dynamic ? b2_dynamicBody : b2_kinematicBody;
    def.bullet = type == type_bullet;
    def.fixedRotation = type == type_character;
    def.active = false; // init as false, activate during first update
    // FIXME: other settings?

    _internal->body = world2d_mgr::instance()._internal->world.CreateBody(&def);
}

void collider2d::reset_shapes(std::initializer_list<shape::init_t> const& shapes,
                              mass const& m) {
    clear_shapes();
    
    for(auto& it : shapes) {
        std::get<0>(it).append_to(this);
    }
    
    if(m.weight > 0.f) {
        b2MassData data { m.weight, {m.x,m.y}, m.rotate};
        _internal->body->SetMassData(&data);
    }
}

void collider2d::clear_shapes() {
    // TODO
}

void collider2d::update_from_transform(com::transform &transform) {
    auto& affine = transform.global_affine();
    affine3f::LinearMatrixType rotMatrix, scaleMatrix;
    affine.computeRotationScaling(&rotMatrix, &scaleMatrix);
    auto translation = affine.translation();
    auto euler = rotMatrix.eulerAngles(0, 1, 2);
    
    if(std::fabs(translation.z() - FLT_EPSILON) > 0.f ||
       std::fabs(euler.x() - FLT_EPSILON) > 0.f ||
       std::fabs(euler.y() - FLT_EPSILON) > 0.f ||
       std::fabs(scaleMatrix(0,0) - 1.f) > FLT_EPSILON ||
       std::fabs(scaleMatrix(1,1) - 1.f) > FLT_EPSILON ||
       std::fabs(scaleMatrix(2,2) - 1.f) > FLT_EPSILON
       ) {
        affine = Eigen::AngleAxisf(euler.z(), vector3f::UnitZ());
        affine.pretranslate(vector3f(translation.x(), translation.y(), 0.f));
        transform.mark_dirty(false); // update the local
    }
    
    _internal->position = {translation.x(), translation.y()};
    _internal->z_angle = euler.z();
    _internal->body->SetTransform(_internal->position, euler.z());
}

void collider2d::apply_to_transform(com::transform &transform) const{
    auto& current = _internal->body->GetPosition();
    auto angle = _internal->body->GetAngle();
    if(current == _internal->position &&
       angle == _internal->z_angle) {
        return;
    }
    
    transform.set_global_affine(Eigen::Translation3f(current.x, current.y, 0.f) *
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
    b2Vec2(gravity.x(), gravity.y()),
    ratio,
}), _velocity_iteration(6), _position_iteration(2){
    
}

void world2d_mgr::update(goes_t const&) {
    auto& world = _internal->world;
    auto offset = com::transform_manager::flag_offset();
    auto transform_idx = com::transform_manager::component_idx();
    
    // FIXME: dont' update detached game objects
    for(b2Body* first = world.GetBodyList(); first; first = first->GetNext()) {
        auto* collider = static_cast<collider2d*>(first->GetUserData());
        auto* obj = collider->parent();
        
        // FIXME: what if it is really deactived?
        if(!first->IsActive() ||
           (obj->flag() >> offset) & com::transform_manager::global_bit) {
            auto* transform = obj->get_component<com::transform>(transform_idx);
            if(transform) {
                collider->update_from_transform(*transform);
            }

            first->SetActive(true);
        }
    }
    
    world.Step(1.f/30.f, _velocity_iteration, _position_iteration);
    
    for(b2Body* first = world.GetBodyList(); first; first = first->GetNext()) {
        if(!first->IsAwake())
            continue;
        
        auto* collider = static_cast<collider2d*>(first->GetUserData());
        auto* obj = collider->parent();
        auto* transform = obj->get_component<com::transform>(transform_idx);
        if(transform) {
            collider->apply_to_transform(*transform);
        }
    }
    
}
