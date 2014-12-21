#include "asset/asset_manager.h"
#include "asset/asset_bundle.h"
#include "common/log.h"

DEFINE_LOGGER(asset_manager, "asset");

#if 0
#pragma mark - asset bundle

asset_bundle::handles_t asset_bundle::all_assets(asset_manager::context const&ctx) const{
    handles_t handles;
    for (auto& name : all_names(ctx)) {
        auto handle(get(name, ctx));
        if (handle) {
            handles.emplace_back(std::move(name), std::move(handle));
        }
    }
    return handles;
}

#endif

#pragma mark - asset manager

asset_manager::asset_manager(context const& ctx)
: asset_collection(ctx) {

}

std::pair<uint32_t, uint32_t> asset_manager::add_from_bundle(asset_bundle *bundle) {
    assert(bundle != nullptr);
    
    LOG_INFO("start to add assets from bundle:" << bundle->name());
    auto assets = bundle->assets();
    uint32_t replaced = 0;
    for (auto& it : assets) {
        auto existed = _assets.find(it.first);
        if (existed != _assets.end()) {
            LOG_INFO("replace asset:" << it.first);
            existed->second = std::move(it.second);
            ++ replaced;
        } else {
            LOG_INFO("add asset: " << it.first);
            _assets.emplace(std::move(it.first), std::move(it.second));
        }
    }
    LOG_INFO("finish (" << assets.size() << ',' << replaced << ")");
    return {assets.size(), replaced};
}

uint32_t asset_manager::remove_from_bundle(asset_bundle *bundle) {
    assert(bundle != nullptr);
    
    auto assets = bundle->assets();
    uint32_t num = 0;
    for (auto& it : assets) {
        num += _assets.erase(it.first);
    }
    return num;
}
