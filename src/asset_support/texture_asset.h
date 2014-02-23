#ifndef _CHAOS3D_ASSET_SUPPORT_ASSET_TEXTURE_H
#define _CHAOS3D_ASSET_SUPPORT_ASSET_TEXTURE_H

#include "re/render_device.h"
#include "asset/asset_bundle.h"
#include "asset/locator_asset_bundle.h"
#include "asset/asset_manager.h"

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

#pragma mark - texture assets from locators

// The asset bundle containing the 'png' file as a texture asset
//
// TODO:
// 1. @2x based on device capacity etc..
// 2. mipmap
// 3. multiple extensions/pvr/png all together?
class png_asset_bundle : public locator_asset_bundle {
public:
    png_asset_bundle(render_device*,
                     asset_locator::ptr const&,
                     std::string const&);

    virtual handle_ptr get(std::string const&) const override;
    
    static ptr bundle(render_device* device, asset_locator::ptr const&,
                      std::string const& = ".png");
    
private:
    render_device *_device;
    texture::attribute_t _attribute; // ubiquitous attribute
};

// TODO
class pvr_asset_bundle : public locator_asset_bundle {
    
};
#endif