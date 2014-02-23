#ifndef _CHAOS3D_JSON_LOADER_H
#define _CHAOS3D_JSON_LOADER_H

#include <memory>

struct cJSON;
class memory_stream;

// The JSON wrapper for cJSON, this serves to remove
// dependencies between the client code and cJSON
class json_loader {
public:
    typedef int position_t;
    
public:
    json_loader(char const*);
    json_loader(memory_stream*);
    
    json_loader(json_loader&& rhs)
    : _json_root(rhs._json_root) {
        rhs._json_root = NULL;
    }
    
    json_loader& operator=(json_loader&& rhs) {
        _json_root = rhs._json_root;
        rhs._json_root = NULL;
        return *this;
    }
    
    ~json_loader();
    
    json_loader(json_loader const&) = delete;
    json_loader& operator=(json_loader const&) = delete;
    
    cJSON* internal() const { return _json_root; }
private:
    cJSON* _json_root;
};

#endif