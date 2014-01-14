#include "transform.h"

using namespace com;

void transform::update(transform const& parent) {
    _global_affine = parent._global_affine;
    _global_affine.pretranslate(_translate).prescale(_scale).prerotate(_rotate);
    _global_reversed = _global_affine.matrix().reverse();
}