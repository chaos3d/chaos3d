#include "asset/asset_locator.h"
#include "io/file_stream.h"
#include "common/log.h"

#include <queue>
#include <algorithm>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

DEFINE_SINGLETON(locator_mgr);
INHERIT_LOGGER(locator_mgr, data_stream);

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
    if (_locators.size() == 0) {
        LOG_WARN("no locators are added, loading: " << name);
        return nullptr;
    }
    
    LOG_TRACE("loading stream: " << name);
    for (auto& it : _locators) {
        LOG_TRACE("searching in:" << it->name());
        auto stream = it->from(name);
        if (stream.get() != nullptr) {
            LOG_TRACE("found");
            return stream;
        }
    }
    LOG_WARN(name << " couldn't be found.");
    return nullptr;
}

namespace locator {
    dir_locator::dir_locator(std::string const& base, int priority)
    : _base(base + '/'), asset_locator(priority) {
        struct stat st;
        
        if(stat(base.c_str(), &st) != 0 ||
           !S_ISDIR(st.st_mode)) {
            LOG_ERROR("directory doesn't exist: " << base);
        }
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
    
    dir_locator::ptr dir_locator::cur_dir(int priority, char const* sub) {
        char* cur_dir = getcwd(NULL, 0);
        // ignore root dir, this happens to iOS
        dir_locator* dir = strcmp(cur_dir, "/") == 0
            ? nullptr
            : new dir_locator(std::string(cur_dir) + sub, priority);
        free(cur_dir);
        return ptr(dir);
    }
}