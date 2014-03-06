#include "script/state.h"
#include "io/data_stream.h"

using namespace script;

static const char *_stream_reader (lua_State *L, void *data, size_t *size){
    static constexpr uint32_t BUF_SIZE = 1024;
	static char buffer[BUF_SIZE];
	data_stream* ds = (data_stream*)data;
    
	*size = ds->read(buffer, BUF_SIZE);
	return buffer;
}

coroutine::coroutine(lua_State* L, state* parent)
: _L(L), _parent(parent->shared_from_this()) {
    
}

coroutine::~coroutine() {
    // TODO: recycle coroutine
}

coroutine& coroutine::resume() {
    if (!is_resumable())
        return *this;
    lua_resume(_L, 0);
    return *this;
}

bool coroutine::is_yielded() const {
    return !_parent.expired() && lua_status(_L) == LUA_YIELD;
}

bool coroutine::is_resumable() const {
    if (_parent.expired())
        return false;
    int st = lua_status(_L);
    return (st == LUA_YIELD || st == 0);
}

state::state(bool open_all)
: _L(luaL_newstate()) {
    if (open_all) {
        luaL_openlibs(_L);
    }
}

state::~state() {
    lua_close(_L);
}

coroutine::ptr state::load(data_stream *ds, char const* name) {
    lua_State* L = lua_newthread(_L); // TODO: coroutine pool
    int ret = lua_load(L, _stream_reader, (void*)ds, name);
    
    if (ret != 0) {
        // TODO: proper log
        printf("compiling errors: %s", lua_tostring(L, -1));
    } 
    return coroutine::ptr(new coroutine(L, this));
}
