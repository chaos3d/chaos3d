#include "asset/asset_locator.h"
#include "io/file_stream.h"

#include <queue>
#include <algorithm>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

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

    bool dir_locator::contains(std::string const& name) const {
        struct stat st;
        auto full = _base + name;
        
        return stat(full.c_str(), &st) == 0 && (S_ISREG(st.st_mode));
    }
    
    data_stream::ptr dir_locator::from(std::string const& name) const {
        struct stat st;
        auto full = _base + name;
        
        if(stat(full.c_str(), &st) != 0 ||
           !S_ISREG(st.st_mode))
            return nullptr;
        
        return data_stream::ptr(new file_stream(full.c_str()));
    }

    void dir_locator::traverse(visitor_t const& visitor) const {        
        std::queue<std::string> dirs;
        dirs.emplace(_base);
        
        while (!dirs.empty()) {
            auto& cur = dirs.front();
            
            struct dirent *entry;
            DIR *dp;
            
            dp = opendir(cur.c_str());
            if (dp == NULL) {
                dirs.pop();
                continue;
            }

            auto base = std::string(cur.data() + _base.length());
            while ((entry = readdir(dp))) {
                if (strcmp(entry->d_name, ".") == 0 ||
                    strcmp(entry->d_name, "..") == 0) {
                    continue;
                } else if (entry->d_type == DT_DIR) {
                    dirs.emplace(cur + entry->d_name + '/');
                } else if (cur.length() == _base.length()) {
                    visitor(entry->d_name);
                } else {
                    visitor(base + entry->d_name);
                }
            }
            
            dirs.pop();
            closedir(dp);
        }
        
    }
}