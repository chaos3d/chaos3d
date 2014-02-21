#include "asset_manager.h"
#include "re/texture.h"

asset_manager::asset_manager()
: _loading_thread(std::bind(&asset_manager::loading_thread, this))
{

}

asset_manager::~asset_manager() {
    // TODO:
    // To signal the loading thread to stop
}

void asset_manager::loading_thread() {
    
}

std::pair<uint32_t, uint32_t> asset_manager::add_from_bundle(asset_bundle *bundle) {
    assert(bundle != nullptr);
    
    auto assets = bundle->all_assets();
    uint32_t replaced = 0;
    for (auto& it : assets) {
        auto existed = _assets.find(it.first);
        if (existed == _assets.end()) {
            existed->second = std::move(it.second);
            ++ replaced;
        } else {
            _assets.emplace(std::move(it.first), std::move(it.second));
        }
    }
    
    return {assets.size(), replaced};
}

uint32_t asset_manager::remove_from_bundle(asset_bundle *bundle) {
    assert(bundle != nullptr);
    
    auto names = bundle->all_names();
    uint32_t num = 0;
    for (auto& it : names) {
        num += _assets.erase(it);
    }
    return num;
}

void asset_manager::load_asset(asset_handle* handle) {
    assert(handle != nullptr);

    handle->load();
}