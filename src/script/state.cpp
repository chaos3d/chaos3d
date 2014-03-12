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
#if 0
    if (!parent().expired()) {
        parent().lock()->recycle(std::move(*this));
    }
#else
    if (parent())
        parent()->recycle(std::move(*this));
#endif
}

bool coroutine::resume_with(int nargs) {
    if (!is_yielded())
        return false;
    int ret = lua_resume(_L, nargs);
    if (ret > 1) {
        // TODO: proper log
        printf("runtime errors: %s", lua_tostring(_L, -1));
    }
    return ret <= 1;
}

bool coroutine::is_yielded() const {
#if 0
    return !parent().expired() && lua_status(_L) == LUA_YIELD;
#else
    int st = lua_status(_L);
    if (st == 0) {
        return lua_isfunction(_L, 1);
    } else
        return st == LUA_YIELD;
#endif
}

#if 0
bool coroutine::is_resumable() const {
    if (parent().expired())
        return false;
    int st = lua_status(_L);
    return (st == LUA_YIELD || st == 0);
}
#endif

state::state(bool open_all)
: _L(luaL_newstate()) {
    ensure_objlink();
    if (open_all) {
        luaL_openlibs(_L);
    }
}

state::~state() {
    lua_close(_L);
}

void state::ensure_objlink() {
    lua_newtable(_L);
    lua_newtable(_L);
    lua_pushstring(_L, "kv");
    lua_setfield(_L, -2, "__mode");
    lua_setmetatable(_L, -2);
#if 0
    // TODO: add debug tracking info for __index/__newindex
#endif
    int ref = lua_ref(_L, 1);
    assert(ref == 1);
}

void state::recycle(script::coroutine &&co) {
    int r = co._co_ref.release();
    _coroutines.emplace_back(std::move(co));
    _co_refs.emplace_back(r);
}

coroutine state::fetch() {
    if (!_coroutines.empty()) {
        coroutine co = std::move(_coroutines.back());
        lua_getref(_L, _co_refs.back());
        
        _coroutines.pop_back();
        _co_refs.pop_back();
        
        co._co_ref.reset(this);
        return co;
    } else {
        lua_newthread(_L);
        return coroutine(this); // transfer ownership to co, the thread is removed from the stack
    }
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

// push the named scope(table) or the global table
void state::push_scope(lua_State* L, char const* scope) {
    if (scope == nullptr) {
        lua_pushvalue(L, LUA_GLOBALSINDEX);
    } else {
        lua_getglobal(L, scope);
        // TODO: recursively create the table
        assert(0);
    }
}

import_scope state::import(char const* name) {
    return import_scope(name, *this);
}

#if 0
state& state::import(void* data, char const* name, type_info const* type, char const* scope) {
    if (type != nullptr) {
        lua_pushlightuserdata(_L, type);
        lua_rawget(L, LUA_REGISTRYINDEX);
        
        if (!lua_istable(_L, -1)) {
            lua_pop(_L, 1); // pop nil
            lua_newtable(_L);
        }
    }
    
    for (auto& it : import.meta()) {
        lua_pushstring(_L, it.first.c_str());
        lua_pushcfunction(_L, it.second);
        lua_rawset(_L, -3);
    }
    return *this;
}
#endif