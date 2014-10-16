#ifndef _CHAOS3D_SCRIPT_TYPE_INFO_H
#define _CHAOS3D_SCRIPT_TYPE_INFO_H

#include <typeinfo>
#include <vector>
#include <unordered_map>
#include <liblua/lua/lua.hpp>
#include "common/referenced_count.h"

namespace script {
    typedef int (*lua_function_t)(lua_State*);
    class type_info;

    // object data saved in userdata
    struct object_wrapper {
        void* object = nullptr;
        type_info* type = nullptr;
    };
    
    // pointers without deleter, this can be dangerous since the data
    // in Lua can be dangling pointers, but like components, it seems
    // to be not easy to avoid, or we need another strategy to import
    // this into Lua
    struct pure_pointer_tag {};
    
    // pointers with deleter, it behaves like regular objects and
    // when it's out of scope, it will be deleted. this is mostly used
    // for the struct-like objects, they can be copy consturcted/assigned
    struct raw_pointer_tag {};
    
    // objectes derived from referenced_count so gc will call release
    struct referenced_count_tag {};

    // shared_ptr, just like raw pointer, but shared_ptr is saved to the
    // upvalue of gc method
    struct shared_pointer_tag {};
    
    // TODO: alternative to save gc info into meta table, we can save into
    // the object fields so we would be able to use one single meta-table
    // and it would less likely to have meta table referencing something
    // outside
    template<class T>
    struct object_meta;
    
    template<>
    struct object_meta<pure_pointer_tag> {
        static int __index(lua_State* L);

        static void retain(void* r) {
        }

        // dummy template to match the signature
        template<class T = void, class D = std::nullptr_t>
        static void push_metatable(lua_State *L, D const& = D()) {
            if(luaL_newmetatable(L, "__pure_pointer") == 1) {
                lua_pushcfunction(L, __index);
                lua_setfield(L, -2, "__index");
            }
        }
    };

    template<>
    struct object_meta <raw_pointer_tag> : public object_meta <pure_pointer_tag>{
        template<class T, class D>
        static int __gc(lua_State* L) {
            object_wrapper* obj = (object_wrapper*) lua_touserdata(L, 1);
            if (obj->object == nullptr)
                return 0;
            
            // we don't support copiable deleter as it may cause memory
            // leaks in Lua, just to simplify the scenario until we actually
            // need it
            D()((T*)obj->object);
            return 0;
        }
        
        template<class T, class D>
        static void push_metatable(lua_State *L, D const&d) {
            if(luaL_newmetatable(L, typeid(T).name()) == 1) {
                lua_pushcfunction(L, __index);
                lua_setfield(L, -2, "__index");
                
                lua_pushcfunction(L, (__gc<T, D>));
                lua_setfield(L, -2, "__gc");
            }
        }
    };

    template<>
    struct object_meta <shared_pointer_tag> : public object_meta <pure_pointer_tag>{
        template<class T>
        static int __gc(lua_State* L) {
            typedef std::shared_ptr<T> ptr_t;
            object_wrapper* obj = (object_wrapper*) lua_touserdata(L, 1);
            if (obj->object == nullptr)
                return 0;

            ptr_t* ptr = (ptr_t*)lua_touserdata(L, lua_upvalueindex(1));
            ptr->~ptr_t();
            return 0;
        }
        
        template<class T>
        static void push_metatable(lua_State *L, std::shared_ptr<T> const& ptr) {
            typedef std::shared_ptr<T> ptr_t;

            // each object will have a unique metatable
            // because shared_ptr bound to gc function
            lua_createtable(L, 0, 2);

            lua_pushcfunction(L, __index);
            lua_setfield(L, -2, "__index");
                
            void* addr = lua_newuserdata(L, sizeof(ptr_t));
            new (addr) ptr_t(ptr);
            lua_pushcclosure(L, __gc<T>, 1);
            lua_setfield(L, -2, "__gc");
        }
    };
    
    template<>
    struct object_meta <referenced_count_tag> : public object_meta <pure_pointer_tag>{
        static void retain(referenced_count* r) {
            r->retain();
        }
        
        static int __gc(lua_State* L) {
            // TODO: log
            object_wrapper* obj = (object_wrapper*) lua_touserdata(L, 1);
            if (obj->object == nullptr)
                return 0;

            referenced_count* r = (referenced_count*)obj->object;
            r->release();
            return 0;
        }
        
        // dummy template to match the signature
        template<class T = void, class D = std::nullptr_t>
        static void push_metatable(lua_State *L, D const& = D()) {
            if(luaL_newmetatable(L, "__reference_tag") == 1) {
                lua_pushcfunction(L, __index);
                lua_setfield(L, -2, "__index");
                
                lua_pushcfunction(L, __gc);
                lua_setfield(L, -2, "__gc");
            }
        }
    };
}
#endif