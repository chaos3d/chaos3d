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

coroutine::coroutine(state* parent)
: _L(lua_tothread(parent->internal(), -1)), _co_ref(parent) {
    
}

coroutine::~coroutine() {
    if (!parent().expired()) {
        parent().lock()->recycle(std::move(*this));
    }
}

coroutine& coroutine::resume() {
    if (!is_resumable())
        return *this;
    lua_resume(_L, 0);
    return *this;
}

bool coroutine::is_yielded() const {
    return !parent().expired() && lua_status(_L) == LUA_YIELD;
}

bool coroutine::is_resumable() const {
    if (parent().expired())
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

void state::recycle(script::coroutine &&) {
    
}

coroutine state::fetch() {
    lua_newthread(_L);
    return coroutine(this); // transfer ownership to co, the thread is removed from the stack
}

coroutine state::load(char const* s, char const* name) {
    coroutine co(fetch());
    auto* L = co.internal();
    
    int ret = luaL_loadbuffer(L, s, strlen(s), name);
    if (ret != 0) {
        // TODO: proper log
        printf("compiling errors: %s", lua_tostring(L, -1));
    }
    return co;
}

coroutine state::load(data_stream *ds, char const* name) {
    coroutine co(fetch());
    auto* L = co.internal();
    
    int ret = lua_load(L, _stream_reader, (void*)ds, name);    
    if (ret != 0) {
        // TODO: proper log
        printf("compiling errors: %s", lua_tostring(L, -1));
    } 
    return co;
}
