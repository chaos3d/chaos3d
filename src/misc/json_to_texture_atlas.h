#ifndef _CHAOS3D_JSON_TO_TEXTURE_ATLAS_H
#define _CHAOS3D_JSON_TO_TEXTURE_ATLAS_H

#include "io/io_converter.h"
#include "io/json_loader.h"
#include "com/sprite2d/texture_atlas.h"

template<>
struct io_converter<json_loader, texture_atlas> {
    bool operator() (json_loader const&, texture_atlas&) const;
};

#endif