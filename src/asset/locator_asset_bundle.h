#ifndef _CHAOS3D_ASSET_LOCATOR_ASSET_BUNDLE_H
#define _CHAOS3D_ASSET_LOCATOR_ASSET_BUNDLE_H

#include "asset/asset_bundle.h"
#include "asset/asset_locator.h"

// The asset bundle from the given path(locator) and the
// extension for a particular file type
// This is a helper bundle for the assets from the files of
// the same type (.png); this may only produce one type
// of resources (texture). The file type itself can be
// a meta or produce several types of resources as well.
class locator_asset_bundle : public asset_bundle {
public:
    virtual names_t all_names() const override;
    
protected:
    locator_asset_bundle(asset_locator::ptr const& locator,
                         std::string const& meta_extension = "");
    
    asset_locator::ptr const& locator() const { return _locator;}
    std::string const& extension() const { return _extension; }
    
private:
    std::string _extension;
    asset_locator::ptr _locator;
};

// TODO: asset handle factory based on type (extension or
// info in the .meta?), so the locator bundle can be load
// from a list of files
#endif