#ifndef _CHAOS3D_ASSET_ASSET_BUNDLE_H
#define _CHAOS3D_ASSET_ASSET_BUNDLE_H

#include <memory>
#include <string>
#include <type_traits>
#include <vector>
#include "common/utility.h"

class asset_handle : public std::enable_shared_from_this<asset_handle> {
public:
    typedef std::shared_ptr<asset_handle> ptr;

public:
    template<class T>
    typename std::remove_cv<T>::type& as() {
        return static_cast<typename std::remove_cv<T>::type&>(*this);
    }
    
    template<class Loader, class... Args>
    bool load_from(Loader&&, Args&&...);
    
    // TODO: thread-safe for async loading
    virtual bool is_loaded() const = 0;
    virtual void load() = 0;
    virtual void unload() = 0;
    
    // weak reference to the actual asset
    // virtual uint32_t use_count() const = 0;
    virtual ~asset_handle() {};
};

// asset_handle placeholder
// any asset needs to specialize this class
// this is to prevent from that any asset would be inheriting
// from the common parent, to remove dependencies
template<class T>
class asset_handle_base;

// asset meta directory
class asset_bundle : public std::enable_shared_from_this<asset_bundle> {
public:
    typedef std::shared_ptr<asset_handle> asset_ptr;
    typedef std::vector<std::pair<std::string, asset_ptr>> assets_t;
    
public:
    virtual ~asset_bundle() {};
    
    // load a new meta from the given name
    // this will also produce a new asset since
    // the meta will be different
    virtual asset_ptr load(std::string const&) const = 0;
    
    // find all the assets meta in this bundle
    virtual assets_t all_assets() const = 0;
    
    // find all the assets names
    virtual std::vector<std::string> all_names() const = 0;
};

#endif