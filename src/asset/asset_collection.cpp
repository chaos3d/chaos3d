#include "asset/asset_collection.h"

asset_collection::asset_collection(context const& ctx) : _context(ctx) {

}

asset_collection::~asset_collection() {
    // TODO:
    // To signal the loading thread to stop
}

void asset_collection::purge() {
    for (auto& it: _assets) {
        if (it.second->is_loaded() && it.second->unique()) {
            it.second->unload();
        }
    }
}

void asset_collection::do_load(asset_handle* handle) {
    assert(handle != nullptr);

    handle->load(*this);
}

bool asset_collection::add(std::string const& name, handle_ptr const& handle, bool override) {
    handles_t::iterator it;
    if (!override || (it = _assets.find(name)) == _assets.end()) {
        LOG_INFO("add asset: " << name);
        return _assets.emplace(name, handle).second;
    } else {
        it->second = handle;
        return true;
    }
}
