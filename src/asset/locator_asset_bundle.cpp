#include "asset/locator_asset_bundle.h"
#include "asset/asset_loader.h"
#include "io/file_stream.h"

locator_asset_bundle::locator_asset_bundle(asset_locator::ptr const& locator,
                                           loaders_t && loaders,
                                           context const& ctx)
: asset_bundle(std::move(loaders), ctx), _locator(locator){
    load_assets();
}

locator_asset_bundle::~locator_asset_bundle() {
    // blank on purpose
}

void locator_asset_bundle::load_assets() {
    _locator->traverse([&] (std::string const& name, data_stream::ptr&& stream) {
        for (auto& loader : loaders()) {
            if (loader->can_load(stream.get())) {
                auto dot = name.find_last_of(".");
                auto split = name.find_last_of("/"); //FIXME: windows use back slash
                if (split == std::string::npos)
                    split = 0;
                if (dot == std::string::npos)
                    dot = name.length();

                // remove the extension
                add(std::string(name.c_str(), dot > split ? dot : name.length()),
                    loader->load(std::move(stream)));

                break;
            }
        }
    });
}
