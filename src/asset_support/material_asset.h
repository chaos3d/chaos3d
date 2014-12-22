#ifndef _CHAOS3D_ASSET_SUPPORT_MATERIAL_ASSET_H
#define _CHAOS3D_ASSET_SUPPORT_MATERIAL_ASSET_H

#include "re/render_device.h"
#include "re/material.h"
#include "asset/asset_handle.h"

// texture asset handle
template<>
class asset_handle_base<material> : public functor_asset_handle<material> {
public:
    typedef functor_asset_handle<material> base_t;

public:
    asset_handle_base(loader_t const& loader)
    : base_t(loader) {
    }

    // TODO: load from a meta config
};

typedef asset_handle_base<material> material_handle;

#endif