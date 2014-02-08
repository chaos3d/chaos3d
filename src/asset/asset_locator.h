#ifndef _ASSET_LOCATOR_H
#define _ASSET_LOCATOR_H

#include <string>

class data_stream;

class asset_locator {
public:
    asset_locator(int priority = 0) : _priority(priority)
    {}
    
    virtual ~asset_locator() {};
    
    virtual std::unique_ptr<data_stream> from(std::string const&) const = 0;
    
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
        dir_locator(std::string const&);
        
        virtual std::unique_ptr<data_stream> from(std::string const&) const;
        
        static dir_locator* app_dir();
        static dir_locator* home_dir();
    private:
        std::string _base;
    };
};
#endif