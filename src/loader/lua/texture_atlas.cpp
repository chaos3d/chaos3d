#include "asset/asset_manager.h"
#include "asset_support/texture_asset.h"
#include "com/sprite2d/texture_atlas.h"
#include "script/lua_ref.h"

template<>
std::unique_ptr<texture_atlas> texture_atlas::load_from(script::ref const& json,
                                                        /* safer to pass a pointer as dereference can easily fail */
                                                        asset_manager* mgr) {
    texture_atlas *atlas = new texture_atlas();
    return std::unique_ptr<texture_atlas>(atlas);
}
