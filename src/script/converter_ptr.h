#ifndef _CHAOS3D_SCRIPT_CONVERTER_PTR_H
#define _CHAOS3D_SCRIPT_CONVERTER_PTR_H

#include <memory>
#include "script/converter.h"
#include "common/referenced_count.h"

namespace script {
    
    template<class P, class D>
    struct converter<std::unique_ptr<P, D>> {
        typedef typename std::remove_cv<P>::type T;

        template<class T>
        using R = std::is_base_of<referenced_count, T>;
        
        // we won't be able to convert back to unique_ptr since
        // we can't explicitly give up the ownership due to GC
        template<class U = P, typename std::enable_if<!R<U>::value>::type* = nullptr>
        static void from(lua_State* L, int idx, char* storage) {
            static_assert(!R<U>::value, "we don't transfer back ownership");
        };

        // referenced count can "generate" unique_ptr
        template<class U = P, typename std::enable_if<R<U>::value>::type* = nullptr>
        static std::unique_ptr<P, referenced_count::release_deleter>
        from(lua_State* L, int idx, char* storage) {
            object_wrapper* obj = (object_wrapper*)lua_touserdata(L, idx);
            // TODO: check the derived type and use DEBUG
            luaL_argcheck(L, obj == nullptr ||  obj->type == &class_<T>::type(), idx,
                          "object type is not matched");
            
            // make sure the object is valid if obj is not null
            assert(obj == nullptr || obj->object != nullptr);
            
            return obj != nullptr ? ((referenced_count*)obj->object)->retain<P>() : nullptr;
        };
        
        // referenced_count object, reuse the meta table
        template<class U = P, typename std::enable_if<R<U>::value>::type* = nullptr>
        static void to(lua_State* L, std::unique_ptr<P, D>&& value) {
            if (!value) {
                lua_pushnil(L);
                return;
            }
            
            void* obj = value.release();
            // state ensures its existence
            lua_getfield(L, LUA_REGISTRYINDEX, "__objlink");
            lua_pushlightuserdata(L, obj);
            lua_rawget(L, -2);
            
            if (lua_isnoneornil(L, -1)) {
                lua_pop(L, 1); // pop the nil
                auto* wrapper = (object_wrapper*)lua_newuserdata(L, sizeof(object_wrapper));
                wrapper->object = obj;
                wrapper->type = &class_<T>::type();
                
                // we take the ownership as a referenced count
                object_meta<referenced_count_tag>::push_metatable<T>(L);
                lua_setmetatable(L, -2);
                lua_pushlightuserdata(L, obj); // # tbl, value, key
                lua_pushvalue(L, -2); // # tbl, value, key, value
                lua_rawset(L, -4); // # tbl, value
            }
            
            // remove the table, the object is already on the stack
            lua_remove(L, -2);
        };

        // we will take over the ownership
        template<class U = P, typename std::enable_if<!R<U>::value>::type* = nullptr>
        static void to(lua_State* L, std::unique_ptr<P, D>&& value) {
            if (!value) {
                lua_pushnil(L);
                return;
            }
            
            void* obj = value.release();
            // state ensures its existence
            lua_getfield(L, LUA_REGISTRYINDEX, "__objlink");
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

    template<class P, class D>
    struct converter<std::unique_ptr<P, D> const> : public converter<std::unique_ptr<P, D>> {
    };
    
    template<class P, class D>
    struct converter<std::unique_ptr<P, D> const&> : public converter<std::unique_ptr<P, D>> {
    };
    
    template<class P>
    struct converter<std::shared_ptr<P>> {
        typedef typename std::remove_cv<P>::type T;
 
        typedef std::enable_shared_from_this<P> S;
        
        template<class T>
        using R = std::is_base_of<std::enable_shared_from_this<T>, T>;

        template<class U = P, typename std::enable_if<!R<U>::value>::type* = nullptr>
        static void from(lua_State* L, int idx, char* storage) {
            static_assert(!R<U>::value, "we don't do this until we save bind shared_ptr to local space");
        };
        
        // referenced count can "generate" unique_ptr
        template<class U = P, typename std::enable_if<R<U>::value>::type* = nullptr>
        static std::shared_ptr<T> from(lua_State* L, int idx, char* storage) {
            object_wrapper* obj = (object_wrapper*)lua_touserdata(L, idx);
            // TODO: check the derived type and use DEBUG
            luaL_argcheck(L, obj == nullptr || obj->type == &class_<T>::type() ||
                          obj->type->is_derived(&class_<T>::type()), idx,
                          "object type is not matched");
            return obj != nullptr ? ((T*)obj->object)->shared_from_this() : nullptr;
        };
        
        // we will take over the ownership
        static void to(lua_State* L, std::shared_ptr<T> const& value) {
            void* obj = value.get();
            if (obj == nullptr) {
                lua_pushnil(L);
                return;
            }
            
            // state ensures its existence
            lua_getfield(L, LUA_REGISTRYINDEX, "__objlink");
            lua_pushlightuserdata(L, obj);
            lua_rawget(L, -2);
            
            if (lua_isnoneornil(L, -1)) {
                lua_pop(L, 1); // pop the nil
                auto* wrapper = (object_wrapper*)lua_newuserdata(L, sizeof(object_wrapper));
                wrapper->object = obj;
                wrapper->type = &class_<T>::type();

                // shared_ptr saved as a upvalue of gc
                // TODO: save it part of the object space
                object_meta<shared_pointer_tag>::push_metatable<T>(L, value);
                lua_setmetatable(L, -2);
                lua_pushlightuserdata(L, obj); // # tbl, value, key
                lua_pushvalue(L, -2); // # tbl, value, key, value
                lua_rawset(L, -4); // # tbl, value
            }
            
            // remove the table, the object is already on the stack
            lua_remove(L, -2);
        }
    };
    
    template<class P>
    struct converter<std::shared_ptr<P> const> : public converter<std::shared_ptr<P>>{
    };
    
    template<class P>
    struct converter<std::shared_ptr<P> const&> : public converter<std::shared_ptr<P>>{
    };
    
}
#endif