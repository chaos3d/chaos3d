#include "transform.h"
#include "go/game_object.h"

using namespace com;

void transform::update(affine3f const& parent) {
    _global_affine = parent;
    _global_affine.translate(_translate).scale(_scale).rotate(_rotate);
    _global_reversed = _global_affine.matrix().reverse();
}

void transform::relocate(game_object* go) {
    auto* com = go->get_component<transform>(transform_manager::component_idx());
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
        
        auto* com = it->get_component<transform>(component_idx());
        if(!com)
            continue;
        
        transform* parent = nullptr;
        if(it->parent())
            parent = it->parent()->get_component<transform>(component_idx());
        
        // if the parent breaks/skips, it won't populate to its descendents
        com->update(parent ? parent->global() : _global_parent);
    }
    
}