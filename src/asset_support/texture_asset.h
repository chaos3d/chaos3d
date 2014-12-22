#ifndef _CHAOS3D_ASSET_SUPPORT_ASSET_TEXTURE_H
#define _CHAOS3D_ASSET_SUPPORT_ASSET_TEXTURE_H

#include "re/render_device.h"
#include "asset/asset_handle.h"

// texture asset handle
template<>
class asset_handle_base<texture> : public functor_asset_handle<texture> {
public:
    typedef functor_asset_handle<texture> base_t;
    
public:
    asset_handle_base(loader_t const& loader)
    : base_t(loader) {
    }
    
    // TODO: load from a meta config
};

typedef asset_handle_base<texture> texture_handle;

#endif