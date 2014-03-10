#include "script/lua_ref.h"
#include "script/state.h"

using namespace script;

ref::ref(state* st)
: _parent(st->shared_from_this()) {
    auto* L = st->internal();
    if (lua_gettop(L) > 0) {
        _ref = lua_ref(L, 1);
    }
}

ref::ref(coroutine& co)
: _parent(co.parent()), _ref(-1) {
//    if (_parent.expired())
//        return;
    
    auto* L = co.internal();
    if (lua_gettop(L) > 0) {
        _ref = lua_ref(L, 1);
    }
}

ref ref::copy() const {
    lua_getref(_parent->internal(), _ref);
#if 1
    return ref(_parent.get());
#else
    if (_parent.expired())
        return ref();
        else {
            return ref(_parent.lock().get());
        }
#endif
}


void ref::release() {
#if 1
    if (_ref != -1) {
        lua_unref(_parent->internal(), _ref);
        _ref = -1;
    }
#else
    if (_ref != -1 && !_parent.expired()) {
        auto* L = _parent.lock()->internal();
        lua_unref(L, _ref);
        _ref = -1;
    }
#endif
}