#ifndef _CHAOS3D_JSON_TO_TEXTURE_ATLAS_H
#define _CHAOS3D_JSON_TO_TEXTURE_ATLAS_H

#include "io/json_loader.h"
#include "com/sprite2d/texture_atlas.h"

template<>
std::pair<bool, json_loader::position_t> texture_atlas::load_from<json_loader>(json_loader const&);
#endif