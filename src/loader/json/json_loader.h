#ifndef _CHAOS3D_LOADER_JSON_JSON_LOADER_H
#define _CHAOS3D_LOADER_JSON_JSON_LOADER_H

#include <memory>

class data_stream;

// The JSON wrapper for cJSON, this serves to remove
// dependencies between the client code and cJSON
class json_loader {
public:
    typedef int position_t;
    
public:
    json_loader(void* internal)
    : _internal(internal) {
    }
    
    template<class T>
    T& internal() { return *reinterpret_cast<T*>(_internal); }

    template<class T>
    T const& internal() const { return *reinterpret_cast<T*>(_internal); }
    
private:
    void* _internal;
};

class json_document : public json_loader {
public:
    json_document(char const*);
    json_document(data_stream*); // TODO: write rapidjson stream reader
    ~json_document();
    
    json_loader& as_json_loader() { return *this; }
private:
    json_document(json_document &&) = delete;
    json_document(json_document const&) = delete;
    json_document& operator=(json_document const&) = delete;
    json_document& operator=(json_document &&) = delete;
};

#endif