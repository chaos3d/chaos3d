#include "asset_support/texture_asset.h"
#include "asset_support/png_loader.h"
#include "io/memory_stream.h"

#if 0
png_asset_bundle::png_asset_bundle(render_device* device,
                                   asset_locator::ptr const& locator,
                                   std::string const& extension)
: locator_asset_bundle(locator, extension), _device(device) {
    assert(_device != nullptr);
}

asset_bundle::handle_ptr png_asset_bundle::get(std::string const& name,
                                               asset_manager::context const& ctx) const {
    // TODO:
    auto asset_name = name + ".png";
    if (!locator()->contains(asset_name))
        return nullptr;

    std::shared_ptr<png_asset_bundle const> thiz(shared_from_this(), this);
    return handle_ptr(new texture_handle([=] (texture::ptr& tex, asset_collection&) {
        // TODO: auto detect the image type and load (A8/RGB565/RGBA8888)
        png_loader png;
        auto img = png.do_load(thiz->locator()->from(asset_name).get(), image_desc::RGBA8888);
        tex = thiz->_device->create_texture(img.desc.size,{
            texture::T2D, texture::RGBA8888,
            texture::Clamp, texture::Clamp,
            texture::NearestLinear, texture::Nearest,
            1
        });
        tex->load(img.data().get(), texture::RGBA8888);
        
        // auto-fill all the mipmaps for png textures
        tex->generate_mipmap();
    }));
}

asset_bundle::ptr png_asset_bundle::bundle(render_device* device, asset_locator::ptr const& locator,
                                           std::string const& ext) {
    return ptr(new png_asset_bundle(device, locator, ext));
}
#endif
