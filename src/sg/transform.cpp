#include "transform.h"
#include "go/game_object.h"

using namespace com;

void transform::update(affine3f const& parent) {
    _global_affine = parent;
    _global_affine.translate(_translate).scale(_scale).rotate(_rotate);
    _global_reversed = _global_affine.matrix().reverse();
}

#pragma mark - the manager
void transform_manager::update(std::vector<game_object*> const& gos) {
    for(auto& it : gos) {
        auto* com = it->get_transform();
        if(!com)
            continue;
        
        transform* parent = nullptr;
        if(it->parent())
            parent = it->parent()->get_transform();
        
        // if the parent breaks/skips, it won't populate to its descendents
        com->update(parent ? parent->global() : _global_parent);
    }
}