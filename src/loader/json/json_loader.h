#ifndef _CHAOS3D_LOADER_JSON_JSON_LOADER_H
#define _CHAOS3D_LOADER_JSON_JSON_LOADER_H

#include <memory>

class memory_stream;

// The JSON wrapper for cJSON, this serves to remove
// dependencies between the client code and cJSON
class json_loader {
public:
    typedef int position_t;
    
public:
    json_loader(char const*);
    json_loader(memory_stream*);
    ~json_loader();
    
    template<class T>
    T const& internal() const { return *reinterpret_cast<T*>(_internal.get()); }
    
private:
    struct internal_t;
    std::unique_ptr<internal_t> _internal;
};

#endif