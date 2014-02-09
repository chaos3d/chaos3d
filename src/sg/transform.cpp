#include "transform.h"
#include "go/game_object.h"

using namespace com;

transform* transform::clone(game_object* go) const {
    transform* com = new transform(go);
    *com = *this;

    // if it belongs to a different parent, re-compute the local
    if(parent()->parent() != go->parent()) {
        com->mark_dirty(false);
    }
    return com;
}

void transform::update_global(affine3f const& parent) {
    _global_affine = parent;
    _global_affine.translate(_translate).scale(_scale).rotate(_rotate);
    _global_reversed = _global_affine.matrix().reverse();
}

void transform::update_local(affine3f const& reversed) {
    affine3f const& local = reversed * global_affine();
    affine3f::LinearMatrixType rotMatrix, scaleMatrix;
    local.computeRotationScaling(&rotMatrix, &scaleMatrix);
    _scale = scaleMatrix.diagonal();
    _rotate = quaternionf(rotMatrix);
    _translate = local.translation();
}

#pragma mark - the manager
transform_manager::transform_manager()
: _global_parent(affine3f::Identity())
{}

void transform_manager::update(std::vector<game_object*> const& gos) {
    auto idx = component_idx();
    auto offset = flag_offset();
    for(auto& it : gos) {
        auto mask = (it->flag() >> offset) & 0x3U;
        if(mask == 0U)
            continue;
        
        auto* com = it->get_component<transform>(idx);
        if(!com)
            continue;
        
        transform* parent = nullptr;
        if(it->parent())
            parent = it->parent()->get_component<transform>();
        
        // if the parent breaks/skips, it won't populate to its descendents
        if(mask & global_bit) { // local takes priorities
            com->update_global(parent ? parent->global_affine() : _global_parent);
        } else {
            com->update_local(parent ? parent->global_reversed() : _global_parent);
        }
    }
}
