#include "transform.h"
#include "go/game_object.h"
#include <cmath>

using namespace com;

transform::transform(game_object* go,
                     vector3f const& translate,
                     quaternionf const& rotate,
                     vector3f const& scale)
: component(go),
_rotate(rotate), _scale(scale), _translate(translate)
{
    if(go->parent()) {
        auto * parent = go->parent()->get_component<transform>();
        update_global(parent ? &parent->global_affine() : nullptr);
    } else
        update_global(nullptr);
}

transform* transform::clone(game_object* go) const {
    transform* com = new transform(go);
    *com = *this;

    // if it belongs to a different parent, re-compute the local
    if(parent()->parent() != go->parent()) {
        com->mark_dirty(false); // FIXME: should compute immediately
    }
    return com;
}

transform& transform::set_rotate_by_axis(float x, float y, float z) {
    _rotate = quaternionf(
                          Eigen::AngleAxisf(x*M_PI/180.f, vector3f::UnitX())
                          * Eigen::AngleAxisf(y*M_PI/180.f,  vector3f::UnitY())
                          * Eigen::AngleAxisf(z*M_PI/180.f, vector3f::UnitZ())
                          );
    return *this;
}

transform& transform::set_skew(float angle_x, float angle_y) {
    _skew = vector3f(std::tan(angle_x*M_PI/180.f), std::tan(angle_y*M_PI/180.f),
                     std::tan(angle_x*M_PI/180.f) * std::tan(angle_y*M_PI/180.f));
    return *this;
}

transform& transform::force_update() {
    auto flag = transform_manager::mask_bit << transform_manager::flag_offset();
    auto idx = transform_manager::component_idx();
    
    // better than call stacks?
    std::forward_list<transform*> parents;
    game_object* go_parent = component::parent();
    assert(go_parent);
    
    // find all parents that need updating (w/o recursive)
    while(go_parent && // TODO: parent maybe game_object::null
          (go_parent->flag() & flag)) {
        auto* transform = go_parent->get_component<com::transform>(idx);
        if(!transform)
            break;
        parents.emplace_front(transform);
        go_parent = go_parent->parent();
    }
    
    // the updated parent matrix
    affine3f* parent = nullptr;
    if(go_parent) {
        auto* transform = go_parent->get_component<com::transform>(idx);
        parent = transform ? &transform->global_affine() : nullptr;
    }
    
    auto global_mask = transform_manager::global_bit << transform_manager::flag_offset();
    
    for(auto transform : parents) {
        auto* go = transform->parent();
        
        if(go->flag() & global_mask) { // local takes priorities
            transform->update_global(parent);
        } else {
            transform->update_local(parent);
        }
        
        parent = &transform->global_affine();
        
        // populate to its children and reset itself
        // so it won't need to update but all the children
        go->populate_flag();
        go->reset_flag(flag, transform_manager::mask_bit);
    }
    
    return *this;
}

void transform::update_global(affine3f const* parent) {
    _global_affine = parent ? *parent : affine3f::Identity();
    if (_skew.x() != 0.f || _skew.y() != 0.f) {
        matrix3f skew;
        skew << 1, _skew.x(), 0,
        _skew.y(), 1 + _skew.z(), 0,
        0, 0, 1;
        _global_affine = _global_affine * skew;
    }
    _global_affine.translate(_translate).scale(_scale).rotate(_rotate);
    _global_inverse = _global_affine.inverse();
}

void transform::update_local(affine3f const* inverse) {
    affine3f const& local = inverse ? *inverse * global_affine() : global_affine();
    affine3f::LinearMatrixType rotMatrix, scaleMatrix;
    local.computeRotationScaling(&rotMatrix, &scaleMatrix);
    _scale = scaleMatrix.diagonal();
    _rotate = quaternionf(rotMatrix);
    _translate = local.translation();
    _skew = vector3f::Zero(); // reset the skew/shear value
}

#pragma mark - the manager
transform_manager::transform_manager()
: _global_parent(affine3f::Identity())
{}

void transform_manager::update(std::vector<game_object*> const& gos) {
    auto idx = component_idx();
    auto global_mask = global_bit << flag_offset();
    auto flag = transform_manager::mask_bit << flag_offset();

    for(auto& it : gos) {
        if((it->flag() & flag) == 0)
            continue;
        
        auto* com = it->get_component<transform>(idx);
        if(!com)
            continue;
        
        transform* parent = nullptr;
        if(it->parent())
            parent = it->parent()->get_component<transform>();
        
        // if the parent breaks/skips, it won't populate to its descendents
        if(it->flag() & global_mask) { // local takes priorities
            com->update_global(parent ? &parent->global_affine() : nullptr);
        } else {
            com->update_local(parent ? &parent->global_inverse() : nullptr);
        }
    }
}
