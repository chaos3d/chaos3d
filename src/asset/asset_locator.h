#ifndef _ASSET_LOCATOR_H
#define _ASSET_LOCATOR_H

class data_stream;
class asset_locator {
public:
    asset_locator(int priority = 0) : _priority(priority)
    {}
    
    virtual ~asset_locator() {};
    
    virtual data_stream* from(std::string const&) const = 0;
    
    // the priority to look up the asset so the 'local' asset
    // will be able to override. probably change to a different
    // approach
    int priority() const { return _priority; }
private:
    int _priority;
};

namespace locator {
    
};
#endif