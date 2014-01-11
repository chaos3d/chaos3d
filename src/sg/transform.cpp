#include "transform.h"

using namespace com;

void transform::update(transform const& parent) {
    _global_affine = parent._global_affine * (_rotate * (_scale * translation3f(_translate)));
    _global_reversed = _global_affine.matrix().reverse();
}