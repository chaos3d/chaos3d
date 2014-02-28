#include "asset/locator_asset_bundle.h"

locator_asset_bundle::locator_asset_bundle(asset_locator::ptr const& locator,
                                           std::string const& meta_extension)
: _locator(locator), _extension(meta_extension) {
    
}

locator_asset_bundle::names_t locator_asset_bundle::all_names(asset_manager::context const&) const {
    names_t names;
    _locator->traverse([&] (std::string const& name) {
        auto pre = name.length() - _extension.length();
        if (name.length() < _extension.length())
            return;
        else if (std::memcmp(_extension.c_str(),
                             name.c_str() + pre, _extension.length()) == 0)
            names.emplace_back(name.c_str(), name.length() - _extension.length());
    });
    return names;
}