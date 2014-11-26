#ifndef _ASSET_LOCATOR_H
#define _ASSET_LOCATOR_H

#include <memory>
#include <string>
#include <vector>
#include "common/singleton.h"
#include "io/data_stream.h"

class asset_locator;

class locator_mgr : public singleton<locator_mgr, Static_Instance> {
public:
    typedef std::shared_ptr<asset_locator const> locator_ptr;
    typedef std::vector<locator_ptr> locators_t;
    
    // the helper struct to sort the locators
    // after a chain of adding
    struct sort_helper {
        locator_mgr* mgr = nullptr;
        locator_mgr* operator->() {
            locator_mgr* next = mgr;
            mgr = nullptr;
            return next;
        }
        
        void disable() {
            mgr = nullptr;
        }
        
        ~sort_helper() {
            if (mgr) {
                mgr->sort_locators();
            }
        }
        
    private:
        explicit sort_helper(locator_mgr* m)
        : mgr(m)
        {}
        
        sort_helper(sort_helper &&rhs)
        : mgr(rhs.mgr) {
            rhs.mgr = nullptr;
        }
        
        sort_helper() = delete;
        sort_helper(sort_helper const&) = delete;
        sort_helper& operator=(sort_helper const&) = delete;
        sort_helper& operator=(sort_helper &&) = delete;
        
        friend class locator_mgr;
    };
    
public:
    // add a new locator
    template<class... Args>
    sort_helper add(Args&&... args) {
        _locators.emplace_back(std::forward<Args>(args)...);
        // remove the null locator
        if (!_locators.back()) {
            _locators.pop_back();
        }
        return sort_helper(this);
    }
    
    // add a new locator without sorting
    // sort_locators need to be called afterwards
    template<class... Args>
    void add_no_sort(Args&&... args) {
        _locators.emplace_back(std::forward<Args>(args)...);

        if (!_locators.back()) {
            _locators.pop_back();
        }
    }

    /// add a locator, it will concatenate the two strings
    /// @param base, @home or @app, the predefined path, or absolute path
    /// @param sub, the subfolder path
    locator_mgr& add_locator(int level, char const* base, char const* sub = "");
    
    // get a stream by looking up the locators by priority
    data_stream::ptr from(std::string const&) const;
    
    // sort locators by priority
    void sort_locators();
    
private:
    locators_t _locators;
};

IMPORT_SINGLETON(locator_mgr);

class asset_locator : public std::enable_shared_from_this<asset_locator> {
public:
    typedef std::shared_ptr<asset_locator const> const_ptr;
    typedef std::shared_ptr<asset_locator> ptr;
    typedef std::function<void(std::string const&)> visitor_t;
    
public:
    virtual ~asset_locator() {};
    
    // check if the given name exists
    virtual bool contains(std::string const&) const = 0;
    
    // get a stream from the given name
    virtual data_stream::ptr from(std::string const&) const = 0;
    
    // go through all the possible names
    virtual void traverse(visitor_t const&) const = 0;
    
    // the name for the locator
    virtual std::string name() const { return "(null)"; }
    
    // the priority to look up the asset so the 'local' asset
    // will be able to override. probably change to a different
    // approach
    int priority() const { return _priority; }

protected:
    asset_locator(int priority = 0)
    : _priority(priority)
    {}

private:
    int _priority;
};

namespace locator {
    class dir_locator : public asset_locator {
    public:
        dir_locator(std::string const&, int = 0);
        
        virtual bool contains(std::string const&) const override;
        virtual data_stream::ptr from(std::string const&) const override;
        virtual void traverse(visitor_t const&) const override;
        virtual std::string name() const override { return _base; }

        static ptr home_dir(int priority = 0, char const* sub = "");
        static ptr app_dir(int priority = 1, char const* sub = "");
        static ptr cur_dir(int priority = 2, char const* sub = "");
    private:
        std::string _base;
    };
};
#endif