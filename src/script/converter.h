#ifndef _CHAOS3D_SCRIPT_CONVERTER_H
#define _CHAOS3D_SCRIPT_CONVERTER_H

#include <type_traits>
#include <string>
#include <vector>
#include <memory>
#include <liblua/lua/lua.hpp>
#include "script/type_info.h"
#include "script/class_type.h"
#include "script/traits.h"
//#include "script/state.h"

namespace script {
    template<class T>
    struct converter {
        template<class C>
        using T0 = typename std::remove_reference<C>::type;
        
        template<class C>
        using T1 = typename std::remove_cv<T0<C>>::type;
        
        template<class C>
        using T2 = typename std::remove_pointer<T1<T0<C>>>::type;
        
        template<class C>
        using is_string = typename std::is_same<T1<T0<C>>, std::string>;
        
        template<typename U = T,
        typename std::enable_if<is_number<T1<U>>::value>::type* = nullptr>
        static T0<U> from(lua_State* L, int idx, char* storage) {
            return (T)lua_tonumber(L, idx);
        };
        
        template<typename U = T,
        typename std::enable_if<std::is_same<T1<U>, char const*>::value>::type* = nullptr>
        static char const* from(lua_State* L, int idx, char* storage) {
            return lua_tostring(L, idx);
        };
        
        template<typename U = T,
        typename std::enable_if<is_string<T1<U>>::value>::type* = nullptr>
        static std::string from(lua_State* L, int idx, char* storage) {
            return std::string(lua_tostring(L, idx));
        };
        
        template<typename U = T,
        typename std::enable_if<std::is_same<T1<T>,
        std::vector<typename vector_of<T1<U>>::type>>::value>::type* = nullptr>
        static T1<U> from(lua_State* L, int idx, char* storage) {
            using R = T1<U>;
            using E = typename vector_of<R>::type;
            if (!lua_istable(L, idx))
                return R();
            
            R result;
            int i = 0;
            while(true) {
                lua_rawgeti(L, idx, ++i);
                if (lua_isnoneornil(L, -1)) {
                    lua_pop(L, 1);
                    break;
                }
                // E will never be a reference
                result.emplace_back(converter<E>::from(L, -1, nullptr));
                lua_pop(L, 1);
            }
            return result;
        };
        
        template<typename U = T,
        typename std::enable_if<is_userdata<T2<U>>::value && std::is_pointer<U>::value
            >::type* = nullptr>
        static T2<U>* from(lua_State* L, int idx, char* storage) {
            object_wrapper* obj = (object_wrapper*)lua_touserdata(L, idx);
            return obj != nullptr ? (T2<U>*)obj->object : nullptr;
        };
        
        template<typename U = T,
        typename std::enable_if<is_userdata<T2<U>>::value && std::is_reference<U>::value
            >::type* = nullptr>
        static T2<U>& from(lua_State* L, int idx, char* storage) {
            object_wrapper* obj = (object_wrapper*)lua_touserdata(L, idx);
            return *(T2<U>*)obj->object;
        };

        template<typename U = T,
        typename std::enable_if<is_number<T1<U>>::value>::type* = nullptr>
        static void to(lua_State* L, T const&& value) {
            lua_pushnumber(L, value);
        }
        
        template<typename U = T,
        typename std::enable_if<std::is_same<T1<U>, char const*>::value>::type* = nullptr>
        static void to(lua_State* L, char const* str) {
            lua_pushstring(L, str);
        };
        
        template<typename U = T,
        typename std::enable_if<std::is_same<typename std::remove_extent<T1<U>>::type, char>::value>::type* = nullptr>
        static void to(lua_State* L, char const* str) {
            lua_pushstring(L, str);
        };
        
        template<typename U = T,
        typename std::enable_if<is_string<T1<U>>::value>::type* = nullptr>
        static void to(lua_State* L, std::string const& value) {
            lua_pushstring(L, value.c_str());
        }
        
        template<typename U = T,
        typename std::enable_if<std::is_same<U, std::vector<typename vector_of<U>::type>>::value>::type* = nullptr>
        static void to(lua_State* L, std::vector<typename vector_of<U>::type> && value) {
            using E = typename vector_of<U>::type;
            lua_newtable(L);
            int i = 0;
            for (auto& it : value) {
                converter<E>::to(L, std::move(it));
                lua_rawseti(L, -2, ++i);
            }
        }
        
        template<typename U = T,
        typename std::enable_if<is_userdata<T1<U>>::value &&
        !std::is_pointer<T1<U>>::value>::type* = nullptr>
        static void to(lua_State* L, T&& value) {
            typedef T2<U> R;
            //to(L, new R(std::forward<R>(value))); // need transfer ownership
            //assert(0);
            lua_getref(L, 1); // state ensures this be 1
            lua_pushlightuserdata(L, &value);
            lua_rawget(L, -2);
            if (lua_isnoneornil(L, -1)) {
                lua_pop(L, 1); // pop the nil
                auto* wrapper = (object_wrapper*)lua_newuserdata(L, sizeof(object_wrapper));
                wrapper->object = &value; // TODO: ownership transfer
                wrapper->type = &class_<T2<U>>::type();
                wrapper->type->push_metatable(L);
                lua_setmetatable(L, -2);
                lua_pushlightuserdata(L, &value); // # tbl, value, key
                lua_pushvalue(L, -2); // # tbl, value, key, value
                lua_rawset(L, -4); // # tbl, value
            }
            
            // remove the table, the object is already on the stack
            lua_remove(L, -2);
        }
        
        template<typename U = T,
        typename std::enable_if<is_userdata<T2<U>>::value &&
        std::is_pointer<T1<U>>::value>::type* = nullptr>
        static void to(lua_State* L, T1<U> value) {
            lua_getref(L, 1); // state ensures this be 1
            lua_pushlightuserdata(L, value);
            lua_rawget(L, -2);
            if (lua_isnoneornil(L, -1)) {
                lua_pop(L, 1); // pop the nil
                auto* wrapper = (object_wrapper*)lua_newuserdata(L, sizeof(object_wrapper));
                wrapper->object = value; // TODO: ownership transfer
                wrapper->type = &class_<T2<U>>::type();
                wrapper->type->push_metatable(L);
                lua_setmetatable(L, -2);
                lua_pushlightuserdata(L, value); // # tbl, value, key
                lua_pushvalue(L, -2); // # tbl, value, key, value
                lua_rawset(L, -4); // # tbl, value
            }
            
            // remove the table, the object is already on the stack
            lua_remove(L, -2);
        }
    };
    
}

#endif