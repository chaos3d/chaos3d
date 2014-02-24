#include "loader/json/json_loader.h"
#include "memory_stream.h"
#include <rapidjson/document.h>

using namespace rapidjson;

json_document::json_document(char const* str)
: json_loader(new Document()) {
    Document& root = internal<Document>();
    if (root.Parse<0>(str).HasParseError()) {
        // TODO: throw exception?
    }
}

json_document::json_document(memory_stream* stream)
: json_document(stream->address()) {
}

json_document::~json_document() {
    delete &internal<Document>();
}