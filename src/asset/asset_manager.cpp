#include "asset/asset_manager.h"
#include "asset/asset_bundle.h"
#include "common/log.h"

DEFINE_LOGGER(asset_manager, "asset");

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

#pragma mark - asset manager

asset_manager::asset_manager(context const& ctx)
: _loading_thread(std::bind(&asset_manager::loading_thread, this)),
_context(ctx)
{

}

asset_manager::~asset_manager() {
    // TODO:
    // To signal the loading thread to stop
}

void asset_manager::loading_thread() {
    
}

bool asset_manager::add(std::string const& name, handle_ptr const& handle, bool override) {
    handles_t::iterator it;
    if (!override || (it = _assets.find(name)) == _assets.end()) {
        return _assets.emplace(name, handle).second;
    } else {
        it->second = handle;
        return true;
    }
}

std::pair<uint32_t, uint32_t> asset_manager::add_from_bundle(asset_bundle *bundle) {
    assert(bundle != nullptr);
    
    auto assets = bundle->all_assets(_context);
    uint32_t replaced = 0;
    for (auto& it : assets) {
        auto existed = _assets.find(it.first);
        if (existed != _assets.end()) {
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
    
    auto names = bundle->all_names(_context);
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

void asset_manager::purge() {
    for (auto& it: _assets) {
        if (it.second->is_loaded() && it.second->unique()) {
            it.second->unload();
        }
    }
}
