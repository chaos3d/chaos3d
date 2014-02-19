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
    typedef std::unique_ptr<asset_locator const> locator_ptr;
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
        return sort_helper(this);
    }
    
    // add a new locator without sorting
    // sort_locators need to be called afterwards
    template<class... Args>
    void add_no_sort(Args&&... args) {
        _locators.emplace_back(std::forward<Args>(args)...);
    }

    // get a stream by looking up the locators by priority
    data_stream::ptr from(std::string const&) const;
    
    // sort locators by priority
    void sort_locators();
    
private:
    locators_t _locators;
};

class asset_locator {
public:
    typedef std::unique_ptr<asset_locator const> ptr;
    
public:
    asset_locator(int priority = 0)
    : _priority(priority)
    {}
    
    virtual ~asset_locator() {};
    
    virtual data_stream::ptr from(std::string const&) const = 0;
    
    // the priority to look up the asset so the 'local' asset
    // will be able to override. probably change to a different
    // approach
    int priority() const { return _priority; }

private:
    int _priority;
};

namespace locator {
    class dir_locator : public asset_locator {
    public:
        dir_locator(std::string const&, int = 0);
        
        virtual data_stream::ptr from(std::string const&) const;
        
        static ptr home_dir(int priority = 0);
        static ptr app_dir(int priority = 1);
    private:
        std::string _base;
    };
};
#endif