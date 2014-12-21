#include "asset/locator_asset_bundle.h"
#include "asset/asset_loader.h"
#include "io/file_stream.h"

locator_asset_bundle::locator_asset_bundle(asset_locator::ptr const& locator,
                                           std::string const& meta_extension)
: _locator(locator), _extension(meta_extension) {
    
}

locator_asset_bundle::~locator_asset_bundle() {
    // blank on purpose
}

locator_asset_bundle::names_t locator_asset_bundle::all_names(asset_manager::context const& ctx) const {
    // TODO: load different assets based on context
    names_t names;
    _locator->traverse([&] (std::string const& name) {
        data_stream::ptr stream(new file_stream(name.c_str()));

        for (auto& loader : _loaders) {
            if (loader->can_load(stream.get())) {
                auto dot = name.find_last_of(".");
                auto split = name.find_last_of("/"); //FIXME: windows use back slash
                if (split == std::string::npos)
                    split = 0;
                if (dot == std::string::npos)
                    dot = name.length();

                // remove the extension
                names.emplace_back(name.c_str(), dot > split ? dot : name.length());
            }
        }

        // TODO: remove those
        auto pre = name.length() - _extension.length();
        if (name.length() < _extension.length()) {
            return;
        } else if (std::memcmp(_extension.c_str(),
                               name.c_str() + pre, _extension.length()) == 0)
            names.emplace_back(name.c_str(), name.length() - _extension.length());
    });
    return names;
}