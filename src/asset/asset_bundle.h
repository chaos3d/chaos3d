#ifndef _CHAOS3D_ASSET_ASSET_BUNDLE_H
#define _CHAOS3D_ASSET_ASSET_BUNDLE_H

#include <memory>
#include <string>
#include <type_traits>
#include <vector>
#include "common/utility.h"
#include "asset/asset_collection.h"
#include "asset/asset_loader.h"

// The assets container: data stream -- asset loader -> asset_handle/asset
// It 'contains' the asset mete info (asset handle) in physical
// form (zip bundle) or virtually (a list of files). It won't
// load or manage the actual asset but create the handle so the
// asset can be lazily loaded by the asset manager
class asset_bundle : public asset_collection, public std::enable_shared_from_this<asset_bundle> {
public:
    typedef std::shared_ptr<asset_bundle> ptr;
    typedef std::shared_ptr<asset_bundle> const_ptr;

    typedef asset_loader::ptr loader_ptr;
    typedef std::forward_list<loader_ptr> loaders_t;

public:
    asset_bundle(loaders_t&& loaders, context const& ctx)
    : _loaders(std::move(loaders)), asset_collection(ctx)
    {};

    virtual ~asset_bundle() {};

    // get bundle name
    virtual std::string name() const { return "(null)"; }

    /// get all the loader
    loaders_t const& loaders() const { return _loaders; }

private:
    loaders_t const _loaders;
};

#endif