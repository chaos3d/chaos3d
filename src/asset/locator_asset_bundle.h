#ifndef _CHAOS3D_ASSET_LOCATOR_ASSET_BUNDLE_H
#define _CHAOS3D_ASSET_LOCATOR_ASSET_BUNDLE_H

#include <memory>
#include <forward_list>
#include "asset/asset_bundle.h"
#include "asset/asset_locator.h"

class asset_loader;

// The asset bundle from the given path(locator) and the
// extension for a particular file type
// This is a helper bundle for the assets from the files of
// the same type (.png); this may only produce one type
// of resources (texture). The file type itself can be
// a meta or produce several types of resources as well.
class locator_asset_bundle : public asset_bundle {
public:
    typedef std::unique_ptr<asset_loader> loader_ptr;
    typedef std::forward_list<loader_ptr> loaders_t;

public:
    /// destructor to remove dependencies on member variables
    virtual ~locator_asset_bundle();

    /// add a loader, release the ownership
    void add_loader(loader_ptr&& loader) {
        _loaders.emplace_front(std::move(loader));
    }

    virtual names_t all_names(asset_manager::context const&) const override;
    
protected:
    locator_asset_bundle(asset_locator::ptr const& locator,
                         std::string const& meta_extension = "");

    loaders_t const& loaders() const { return _loaders; }
    asset_locator::ptr const& locator() const { return _locator;}
    std::string const& extension() const { return _extension; }
    virtual std::string name() const override { return _locator->name(); }
    
private:
    loaders_t _loaders;
    std::string _extension;
    asset_locator::ptr _locator;

    // TODO: preallocate the loaders to the assets?
};

// TODO: asset handle factory based on type (extension or
// info in the .meta?), so the locator bundle can be load
// from a list of files
#endif