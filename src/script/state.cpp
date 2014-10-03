#include "script/state.h"
#include "io/data_stream.h"
#include "common/log.h"

using namespace script;

DEFINE_LOGGER(state, "script");
INHERIT_LOGGER(coroutine, state);

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
        // TODO: track stack
        LOG_ERROR("Runtime error:" << lua_tostring(_L, -1));
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

static void *l_alloc (void *ud, void *ptr, size_t osize,
                      size_t nsize) {
    (void)ud;  (void)osize;  /* not used */
    if (nsize == 0) {
        free(ptr);
        return NULL;
    }
    else
        return realloc(ptr, nsize);
}

state::state(lua_State* L, bool open_all)
: _managed_L(L == nullptr) {
    if (L == nullptr) {
        _L = lua_newstate(l_alloc, this);
    } else {
        void *ud(nullptr);
        lua_Alloc alloc_ = lua_getallocf(L, &ud);

        if (ud != nullptr) {
            LOG_ERROR("the userdata has been set for the given state. "
                      "this may cause misbehaviors since it'll be overriden.");
        }
        
        lua_setallocf(L, alloc_, this);
        _L = L;
    }
#pragma mark - TODO: fix this cpath for iOS build
#if 0
    // link with -exported_symbols_list
    // strip -x => keep global symbols
    lua_getglobal(_L, "package");
    lua_getfield(_L, -1, "cpath");
    lua_pushstring(_L, ";path_to_binary");
    lua_concat(_L, 2);
    lua_setfield(_L, -2, "cpath");
#endif
    ensure_objlink();
    if (open_all) {
        luaL_openlibs(_L);
    }
}

state::~state() {
    if (_managed_L)
        lua_close(_L);
}

void state::ensure_objlink() {
    lua_newtable(_L);
    lua_newtable(_L);
    lua_pushstring(_L, "kv");       // #T, M, "kv"
    lua_setfield(_L, -2, "__mode"); // #T, M (["__mode"] = "kv")
    lua_setmetatable(_L, -2);       // #T
#if 0
    // TODO: add debug tracking info for __index/__newindex
#endif
    lua_setfield(_L, LUA_REGISTRYINDEX, "__objlink");
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
        LOG_ERROR("Compiling errors: " << lua_tostring(L, -1));
    }
    return co;
}

coroutine state::load(data_stream *ds, char const* name) {
    coroutine co(fetch());
    if (ds == nullptr)
        return co;
    
    auto* L = co.internal();
    int ret = lua_load(L, _stream_reader, (void*)ds, name);
    if (ret != 0) {
        LOG_ERROR("Compiling errors: " << lua_tostring(L, -1));
    } 
    return co;
}

int state::memory_usage() {
    return (lua_gc(_L, LUA_GCCOUNT, 0) << 10) | lua_gc(_L, LUA_GCCOUNTB, 0);
}

void state::gc_collect() {
    lua_gc(_L, LUA_GCCOLLECT, 0);
}

void state::gc_step(int step) {
    lua_gc(_L, LUA_GCSTEP, step);
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