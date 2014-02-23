#include "io/json_loader.h"
#include "memory_stream.h"
#include <rapidjson/document.h>

using namespace rapidjson;

struct json_loader::internal_t : public rapidjson::Document {
};

json_loader::json_loader(char const* str)
: _internal(new internal_t()) {
    if (_internal->Parse<0>(str).HasParseError()) {
        // TODO: throw exception?
    }
}

json_loader::json_loader(memory_stream* stream)
: json_loader(stream->address()) { 
}

json_loader::~json_loader() {
}