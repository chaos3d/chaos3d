#ifndef _CHAOS3D_ASSET_SUPPORT_ASSET_TEXTURE_H
#define _CHAOS3D_ASSET_SUPPORT_ASSET_TEXTURE_H

#include "re/texture.h"
#include "asset/asset_bundle.h"

template<>
class asset_handle_base<texture> : public asset_handle {
public:
    texture* get_asset() const;
    
protected:
    virtual bool is_loaded() const;
    virtual void load();
    virtual void unload();

private:
    texture::ptr _texture;
    
    // TODO:
    // 1. where to load?
    // 2. texture meta?
    // 3. per-device?
};

typedef asset_handle_base<texture> texture_handle;

#endif