#ifndef _CHAOS3D_ASSET_ASSET_BUNDLE_H
#define _CHAOS3D_ASSET_ASSET_BUNDLE_H

#include <memory>
#include <string>
#include <type_traits>
#include <vector>
#include "common/utility.h"
#include "asset/asset_manager.h"
#include "asset/asset_handle.h"

// The assets container.
// It 'contains' the asset mete info (asset handle) in physical
// form (zip bundle) or virtually (a list of files). It won't
// load or manage the actual asset but create the handle so the
// asset can be lazily loaded by the asset manager
class asset_bundle : public std::enable_shared_from_this<asset_bundle> {
public:
    typedef std::shared_ptr<asset_bundle> ptr;
    typedef std::shared_ptr<asset_bundle> const_ptr;

    typedef asset_handle::ptr handle_ptr;
    typedef std::vector<std::pair<std::string, handle_ptr>> handles_t;
    typedef std::vector<std::string> names_t;
    
public:
    virtual ~asset_bundle() {};
    
    // get a new meta from the given name, the asset shouldn't
    // be loaded (asset_handle == nullptr)
    // this will also produce a new asset since
    // the meta will be different
    virtual handle_ptr get(std::string const&, asset_manager::context const&) const = 0;
    
    // find all the assets names
    virtual names_t all_names(asset_manager::context const&) const = 0;

    // get all the assets meta in this bundle
    virtual handles_t all_assets(asset_manager::context const&) const;
    
    // get bundle name
    virtual std::string name() const { return "(null)"; }
    
protected:
    asset_bundle() = default;
    
};

#endif