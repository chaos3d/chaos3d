#include "asset/asset_locator.h"
#include "io/file_stream.h"

#include <algorithm>
#include <unistd.h>
#include <sys/stat.h>

struct priority_sorter {
    bool operator() (locator_mgr::locator_ptr const& lhs,
                     locator_mgr::locator_ptr const& rhs) const {
        return lhs->priority() < rhs->priority();
    }
};

void locator_mgr::sort_locators() {
    std::sort(_locators.begin(), _locators.end(), priority_sorter());
}

data_stream::ptr locator_mgr::from(std::string const& name) const {
    assert(std::is_sorted(_locators.begin(), _locators.end(), priority_sorter()));
    for (auto& it : _locators) {
        auto stream = it->from(name);
        if (stream.get() != nullptr)
            return stream;
    }
    return nullptr;
}

namespace locator {
    dir_locator::dir_locator(std::string const& base, int priority)
    : _base(base + '/'), asset_locator(priority) {
        struct stat st;
        
        if(stat(base.c_str(), &st) != 0 ||
           !S_ISDIR(st.st_mode))
            throw std::exception(); // TODO log error or throw exception?
    }
    
    data_stream::ptr dir_locator::from(std::string const& name) const {
        struct stat st;
        auto full = _base + name;
        
        if(stat(full.c_str(), &st) != 0 ||
           !S_ISREG(st.st_mode))
            return nullptr;
        
        return data_stream::ptr(new file_stream(full.c_str()));
    }

}