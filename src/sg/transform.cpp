#include "transform.h"
#include "go/game_object.h"

using namespace com;

transform* transform::clone(game_object* go) const {
    transform* com = new transform(go);
    *com = *this;

    // if it belongs to a different parent, re-compute the local
    if(!is_dirty() && parent()->parent() != go->parent()) {
        com->relocate(go);
    }
    return com;
}

void transform::update(affine3f const& parent) {
    _global_affine = parent;
    _global_affine.translate(_translate).scale(_scale).rotate(_rotate);
    _global_reversed = _global_affine.matrix().reverse();
}

void transform::relocate(game_object* go) {
    auto* com = go->get_component<transform>();
    affine3f const& local = com ? com->global_reversed() * global() : global();
    affine3f::LinearMatrixType rotMatrix, scaleMatrix;
    local.computeRotationScaling(&rotMatrix, &scaleMatrix);
    _scale = scaleMatrix.diagonal();
    _rotate = quaternionf(rotMatrix);
    _translate = local.translation();
}

#pragma mark - the manager
void transform_manager::update(std::vector<game_object*> const& gos) {
    for(auto& it : gos) {
        if(!it->is_set(flag_offset()))
            continue;
        
        auto* com = it->get_component<transform>();
        if(!com)
            continue;
        
        transform* parent = nullptr;
        if(it->parent())
            parent = it->parent()->get_component<transform>();
        
        // if the parent breaks/skips, it won't populate to its descendents
        com->update(parent ? parent->global() : _global_parent);
    }
    
}