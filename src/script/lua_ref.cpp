#include "script/lua_ref.h"

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
    if (_parent.expired())
        return;
    
    auto* L = co.internal();
    if (lua_gettop(L) > 0) {
        _ref = lua_ref(L, 1);
    }
}

void ref::release() {
    if (_ref != -1 && !_parent.expired()) {
        auto* L = _parent.lock()->internal();
        lua_unref(L, _ref);
        _ref = -1;
    }
}