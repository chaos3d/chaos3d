#ifndef _CHAOS3D_SCRIPT_CONVERTER_PTR_H
#define _CHAOS3D_SCRIPT_CONVERTER_PTR_H

#include <memory>
#include "script/converter.h"

namespace script {
    
    template<class P, class D>
    struct converter<std::unique_ptr<P, D>> {
        typedef typename std::remove_cv<P>::type T;

        // we won't be able to convert back to unique_ptr since
        // we can't explicitly give up the ownership due to GC
        static T* from(lua_State* L, int idx, char* storage) {
            auto* obj = (object_wrapper*)lua_touserdata(L, idx);
            return (T*)obj->object;
        };
        
        // we will take over the ownership
        static void to(lua_State* L, std::unique_ptr<P, D>&& value) {
            void* obj = value.release();
            lua_getref(L, 1);
            
            lua_pushlightuserdata(L, obj);
            lua_rawget(L, -2);
            
            if (lua_isnoneornil(L, -1)) {
                lua_pop(L, 1); // pop the nil
                auto* wrapper = (object_wrapper*)lua_newuserdata(L, sizeof(object_wrapper));
                wrapper->object = obj;
                wrapper->type = &class_<T>::type();
                
                // we take the ownership as a regular pointer
                // and use the customized deleter
                object_meta<raw_pointer_tag>::push_metatable<T>(L, value.get_deleter());
                lua_setmetatable(L, -2);
                lua_pushlightuserdata(L, obj); // # tbl, value, key
                lua_pushvalue(L, -2); // # tbl, value, key, value
                lua_rawset(L, -4); // # tbl, value
            }
            
            // remove the table, the object is already on the stack
            lua_remove(L, -2);
        };
    };
    
}
#endif