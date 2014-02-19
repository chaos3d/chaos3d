#include "misc/json_to_components.h"

bool io_converter<json_loader, sprite2d::camera2d>::
operator()(json_loader const&, sprite2d::camera2d &) const {
    return false;
}