#ifndef _CHAOS3D_ASSET_ASSET_HANDLE_H
#define _CHAOS3D_ASSET_ASSET_HANDLE_H

#include <memory>
#include <type_traits>

class referenced_count;
class asset_manager;

// The internal handle in the asset manager so the asset it manages
// can be loaded/unloaded per se. The end user should not deal with
// this class but through the asset manager interface to retrieve the
// actual asset/resource
// The same asset handle can exist in different bundles and points to
// the same asset (i.e. downloaded bundle and loaded to asset manager).
class asset_handle : public std::enable_shared_from_this<asset_handle> {
public:
    typedef std::shared_ptr<asset_handle> ptr;
    
public:
    // helper function to downcast
    template<class T>
    typename std::remove_cv<T>::type& as() {
        return static_cast<typename std::remove_cv<T>::type&>(*this);
    }
    
    // TODO: thread-safe for async loading
    virtual bool is_loaded() const = 0;
    
    // whether the loaded asset is the only reference,
    // so it would be safe to unload
    virtual bool unique() const = 0;
    
    virtual ~asset_handle() {};

protected:
    // load/unload the heavy data from the disk
    virtual void load() = 0;
    virtual void unload() = 0;

    friend class asset_manager;
};

// Helper class for asset handle that manages the resource
// derived from referenced_count and uses a loader lambda
template<class T>
class functor_asset_handle : public asset_handle {
public:
    typedef typename T::ptr ptr_t;
    typedef std::function<void (ptr_t&)> loader_t;
    
public:
    functor_asset_handle(loader_t const& loader)
    : _loader(loader) {
    }
    
    virtual bool is_loaded() const override {
        return _asset_ptr.get() != nullptr;
    }
    
    typename std::enable_if<std::is_base_of<referenced_count, T>::value, ptr_t>::type
    get_asset() const {
        assert(_asset_ptr.get() != nullptr); // TODO: throw?
        return _asset_ptr->template retain<T>();
    }
    
    virtual typename std::enable_if<std::is_base_of<referenced_count, T>::value, bool>::type
    unique() const override {
        return _asset_ptr->ref_count() == 1;
    }
    
protected:
    virtual void load() override {
        if (is_loaded())
            return;
        _loader(_asset_ptr);
    }
    
    virtual void unload() override {
        _asset_ptr.reset();
    }
    
protected:
    ptr_t _asset_ptr;
    loader_t _loader;
};

// asset_handle placeholder, any asset needs to specialize this class
// this is to prevent from that any asset would be inheriting
// from the common parent, to remove dependencies
template<class T>
class asset_handle_base;

#endif