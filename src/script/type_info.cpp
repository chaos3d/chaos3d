#include "script/type_info.h"

using namespace script;

int object_meta <pure_pointer_tag>::__index(lua_State* L) {
    object_wrapper* obj = (object_wrapper*) lua_touserdata(L, 1);
    if (obj->object == nullptr)
        return 0;
    
    auto func = obj->type->find(lua_tostring(L, 2));
    if (func != nullptr) {
        lua_pushcfunction(L, func);
        return 1;
    }
    
    return 0;
}
