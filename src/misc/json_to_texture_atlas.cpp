#include "misc/json_to_texture_atlas.h"

bool io_converter<json_loader, texture_atlas>::
operator()(json_loader const&, texture_atlas &) const {
    return false;
}