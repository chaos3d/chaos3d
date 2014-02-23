#include "io/json_loader.h"
#include "memory_stream.h"
#include <cJSON/cJSON.h>

json_loader::json_loader(char const* str) {
    const char* parse_end = nullptr;
    _json_root = cJSON_ParseWithOpts(str, &parse_end, 1);
    
    // TODO: throw instead?
    assert(parse_end == str + strlen(str));
}

json_loader::json_loader(memory_stream* stream)
: json_loader(stream->address()) { 
}

json_loader::~json_loader() {
    if (_json_root != NULL)
        cJSON_Delete(_json_root);
}