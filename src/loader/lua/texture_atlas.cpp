#include "asset/asset_manager.h"
#include "asset_support/texture_asset.h"
#include "com/sprite2d/texture_atlas.h"
#include "script/lua_ref.h"

template<>
texture_atlas::ptr texture_atlas::load_from(script::ref const& json,
                                            asset_manager& mgr) {
    texture_atlas *atlas = new texture_atlas();
    return ptr(atlas);
}
