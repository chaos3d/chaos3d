#include "asset_support/texture_asset.h"
#include "asset_support/png_loader.h"
#include "io/memory_stream.h"

png_asset_bundle::png_asset_bundle(render_device* device,
                                   asset_locator::ptr const& locator,
                                   std::string const& extension)
: locator_asset_bundle(locator, extension), _device(device) {
}

asset_bundle::handle_ptr png_asset_bundle::get(std::string const&name) const {
    auto asset_name = name + extension();
    if (!locator()->contains(asset_name))
        return nullptr;
    
    auto locator_ = locator(); // copy the locator
    return handle_ptr(new texture_handle([=] (texture::ptr& tex) {
        // TODO: auto detect the image type and load (A8/RGB565/RGBA8888)
        png_loader png(locator_->from(asset_name).get(), image_desc::RGBA8888);
        tex = _device->create_texture(png.image().size,{
            texture::T2D, texture::RGBA8888,
            texture::Clamp, texture::Clamp,
            texture::NearestLinear, texture::Nearest,
            1
        });
        tex->load(png.data().get(), texture::RGBA8888);
    }));
}

asset_bundle::ptr png_asset_bundle::bundle(render_device* device, asset_locator::ptr const& locator,
                                           std::string const& ext) {
    return ptr(new png_asset_bundle(device, locator, ext));
}
