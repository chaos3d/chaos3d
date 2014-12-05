#include "common/log.h"
#include "sg/transform.h"
#include "com/render/camera.h"
#include "com/scene3d/collider3d.h"
#include "com/sprite2d/quad_sprite.h"

#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btConvex2dShape.h>
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

using namespace scene3d;
typedef std::unique_ptr<btCollisionShape> shape_ptr;

struct collider3d::internal {
    std::forward_list<shape_ptr> shapes;
    std::unique_ptr<btCollisionObject> collision_object;
};

struct world3d_mgr::internal {
    std::unique_ptr<btCollisionConfiguration> configuration;
    std::unique_ptr<btDispatcher> dispatcher;
    std::unique_ptr<btBroadphaseInterface> broadphase;
    std::unique_ptr<btCollisionWorld> world;
};

collider3d::collider3d(game_object* go)
 : component(go), _internal(new internal()) {
     auto* obj = new btCollisionObject();
     obj->getWorldTransform().setIdentity();
     obj->setUserPointer(this);
     
     _internal->collision_object.reset(obj);
}

collider3d& collider3d::operator=(collider3d const& rhs){
    LOG_ERROR("no support for cloning collider2d");
    assert(0);
    return *this;
}

void collider3d::destroy() {
    _internal->collision_object->setUserPointer(nullptr);
    world3d_mgr::instance()._internal->world->removeCollisionObject(_internal->collision_object.get());
}

template<>
collider3d& collider3d::reset_from<sprite2d::quad_sprite>() {
    auto* quad = parent()->get_component<sprite2d::quad_sprite>();
    if (!quad) {
        LOG_WARN("no sprite component is found");
        return *this;
    }
    
    auto* obj = _internal->collision_object.get();
    if (obj->getCollisionShape()) {
        world3d_mgr::instance()._internal->world->removeCollisionObject(obj);
    }
    
    // build convex hull shape from the bounding box
    auto& bound = quad->bound();
    btVector3 vertices[] = {
        {bound[0].x(), bound[0].y(), 0},
        {bound[1].x(), bound[1].y(), 0},
        {bound[2].x(), bound[2].y(), 0},
        {bound[3].x(), bound[3].y(), 0},
    };
    auto* convex = new btConvexHullShape(&vertices[0].getX(), 4);
    auto* convex2d = new btConvex2dShape(convex);
    _internal->shapes.emplace_front(convex);
    _internal->shapes.emplace_front(convex2d);

    obj->setCollisionShape(convex2d);
    world3d_mgr::instance()._internal->world->addCollisionObject(obj);
    return *this;
}

void collider3d::update_from_transform(com::transform const& tran) {
    // FIXME: no scaling
    auto& affine = tran.global_affine();
    affine3f::LinearMatrixType rotMatrix, scaleMatrix;
    affine.computeRotationScaling(&rotMatrix, &scaleMatrix);
    auto translation = affine.translation();
    
    _internal->collision_object->setWorldTransform(btTransform(btMatrix3x3(rotMatrix.row(0).x(),
                                                                           rotMatrix.row(0).y(),
                                                                           rotMatrix.row(0).z(),
                                                                           rotMatrix.row(1).x(),
                                                                           rotMatrix.row(1).y(),
                                                                           rotMatrix.row(1).z(),
                                                                           rotMatrix.row(2).x(),
                                                                           rotMatrix.row(2).y(),
                                                                           rotMatrix.row(2).z()),
                                                               btVector3(translation.x(),
                                                                         translation.y(),
                                                                         translation.z()))
                                                   );
}

world3d_mgr::world3d_mgr()
: _internal(new internal()) {
    // TODO: parameterize those configs
    btVector3 worldAabbMin(-1000,-1000,-1000);
    btVector3 worldAabbMax(1000,1000,1000);
    
    _internal->configuration.reset(new btDefaultCollisionConfiguration());
    _internal->dispatcher.reset(new btCollisionDispatcher(_internal->configuration.get()));
    _internal->broadphase.reset(new btAxisSweep3(worldAabbMin,worldAabbMax));
    
    _internal->world.reset(new btCollisionWorld(_internal->dispatcher.get(),
                                                _internal->broadphase.get(),
                                                _internal->configuration.get()));
}

void world3d_mgr::query(com::camera const& cam, vector2f const& pos) {
    vector3f from(cam.unproject(cam.get_target()->normalize_position({pos[0], pos[1], 0.f}, cam.viewport())));
    vector3f to(cam.unproject(cam.get_target()->normalize_position({pos[0], pos[1], 1.f}, cam.viewport())));
    
    btVector3 btFrom(from.x(), from.y(), from.z()), btTo(to.x(), to.y(), to.z());
    btCollisionWorld::ClosestRayResultCallback closestResults(btFrom, btTo);
    closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

    _internal->world->rayTest(btFrom, btTo, closestResults);
    if (closestResults.hasHit()) {
        collider3d* c = (collider3d*)closestResults.m_collisionObject->getUserPointer();
        LOG_DEBUG("hit : " << c->parent()->tag());
    }
}

void world3d_mgr::update(goes_t const& goes) {
    auto& world = *_internal->world;
    auto offset = com::transform_manager::flag_offset();
    auto transform_idx = com::transform_manager::component_idx();
    auto mark = goes.front()->mark();
    
    auto& objects = world.getCollisionObjectArray();
    for (int i = 0; i < objects.size(); ++i) {
        auto* object = objects.at(i);
        auto* collider = static_cast<collider3d*>(object->getUserPointer());
        auto* go = collider->parent();
        
        // deactivate the hidden objects
        if (go->mark() != mark) {
            if (object->isActive())
                object->setActivationState(DISABLE_SIMULATION);
            continue;
        }
        
        // FIXME: what if it is really deactived?
        if (!object->isActive() ||
            (go->flag() >> offset) & com::transform_manager::global_bit) {
            auto* transform = go->get_component<com::transform>(transform_idx);
            if (transform) {
                collider->update_from_transform(*transform);
                _internal->world->updateSingleAabb(collider->_internal->collision_object.get());
            }
            
            object->activate();
            
#if 0
            // FIXME
            for (auto* contact = first->GetContactList();
                 contact != nullptr; contact = contact->next) {
                contact->other->SetAwake(true); // awake the other body
            }
#endif
        }
    }
}

void world3d_mgr::pre_update(goes_t const&) {
//    auto& world = _internal->world;
//    auto transform_idx = com::transform_manager::component_idx();
}
