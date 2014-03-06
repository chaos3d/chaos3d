#include "loader/json/json_loader.h"
#include "data_stream.h"
#include <rapidjson/document.h>

using namespace rapidjson;

class json_ds_wrapper {
public:
    json_ds_wrapper(data_stream* ds)
    : _ds(ds) {
        Read();
    }
    
	char Peek() const { return _current; }
	char Take() { char c = _current; Read(); return c; }
	size_t Tell() const { return _ds->size(); }
    
	// Not implemented
	void Put(char c) {  }
	char* PutBegin() { return 0; }
	size_t PutEnd(char*) { return 0; }
    
private:
    void Read() {
        if (_ds->read(&_current, 1) == 0)
            _current = '\0';
    }
    
    data_stream* _ds;
    char _current;
};

json_document::json_document(char const* str)
: json_loader(new Document()) {
    Document& root = internal<Document>();
    if (root.Parse<0>(str).HasParseError()) {
        // TODO: throw exception?
    }
}

json_document::json_document(data_stream* stream)
: json_loader(new Document()) {
    Document& root = internal<Document>();
    auto wrapper = json_ds_wrapper(stream);
    if (root.ParseStream<0>(wrapper).HasParseError()) {
        // TODO: throw exception?
    }
}

json_document::~json_document() {
    delete &internal<Document>();
}