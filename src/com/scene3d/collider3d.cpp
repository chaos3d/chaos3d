#include "com/scene3d/collider3d.h"
#include "sg/transform.h"
#include <btBulletCollisionCommon.h>

using namespace scene3d;

struct collider3d::internal {
    std::unique_ptr<btCollisionObject> collision_object;
};

collider3d::collider3d(game_object* go)
 : component(go), _internal(new internal()) {

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

struct world3d_mgr::internal {
    std::unique_ptr<btCollisionConfiguration> configuration;
    std::unique_ptr<btDispatcher> dispatcher;
    std::unique_ptr<btBroadphaseInterface> broadphase;
    std::unique_ptr<btCollisionWorld> world;
};

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
