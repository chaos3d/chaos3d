#ifndef _CHAOS3D_JSON_LOADER_H
#define _CHAOS3D_JSON_LOADER_H

#include <memory>
#include "io/data_stream.h"

// TODO: loader interface
class json_loader {
public:
    // take the stream as the fundamental data source
    template<class... Args>
    json_loader(Args&&... args)
    : _stream(std::forward<Args>(args)...)
    {}
    
private:
    data_stream::ptr _stream;
};

#endif