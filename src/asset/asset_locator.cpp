#include "asset/asset_locator.h"
#include "io/file_stream.h"

#include <unistd.h>
#include <sys/stat.h>

namespace locator {
    dir_locator::dir_locator(std::string const& base) : _base(base + '/') {
        struct stat st;
        
        if(stat(base.c_str(), &st) != 0 ||
           !S_ISDIR(st.st_mode))
            throw std::exception(); // TODO log error or throw exception?
    }
    
    std::unique_ptr<data_stream> dir_locator::from(std::string const& name) const {
        struct stat st;
        auto full = _base + name;
        
        if(stat(full.c_str(), &st) != 0 ||
           !S_ISREG(st.st_mode))
            return nullptr;
        
        return std::unique_ptr<data_stream>(new file_stream(full.c_str()));
    }

}