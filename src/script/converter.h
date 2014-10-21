#ifndef _CHAOS3D_SCRIPT_CONVERTER_H
#define _CHAOS3D_SCRIPT_CONVERTER_H

#include <type_traits>
#include <string>
#include <vector>
#include <list>
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
        typename std::enable_if<std::is_same<T1<U>, ref>::value>::type* = nullptr>
        static ref from(lua_State* L, int idx, char* storage) {
            lua_pushvalue(L, idx);
            return ref(L);
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
        typename std::enable_if<
        std::is_same<T1<T>, std::vector<typename vector_of<T1<U>>::type>>::value ||
        std::is_same<T1<T>, std::list<typename vector_of<T1<U>>::type>>::value
        >::type* = nullptr>
        static T1<U> from(lua_State* L, int idx, char* storage) {
            using R = T1<U>;
            using E = typename vector_of<R>::type;
            luaL_argcheck(L, lua_istable(L, idx), idx, "expect a table");
            
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
        typename std::enable_if<is_userdata<T2<U>>::value && std::is_pointer<T1<U>>::value
            >::type* = nullptr>
        static T1<U> from(lua_State* L, int idx, char* storage) {
            object_wrapper* obj = (object_wrapper*)lua_touserdata(L, idx);
            return obj != nullptr ? (T1<U>)obj->object : nullptr;
        };
        
        template<typename U = T,
        typename std::enable_if<is_userdata<T2<U>>::value && !std::is_pointer<T1<U>>::value
            >::type* = nullptr>
        static U from(lua_State* L, int idx, char* storage) {
            object_wrapper* obj = (object_wrapper*)lua_touserdata(L, idx);
            luaL_argcheck(L, obj != nullptr && obj->object != nullptr, idx, "expect an object");
            assert(obj != nullptr && obj->object != nullptr);
            assert(obj->type != nullptr);

#ifdef DEBUG
            luaL_argcheck(L, obj->type == &class_<T2<U>>::type() ||
                          obj->type->is_derived(&class_<T2<U>>::type()), idx,
                          "object type is not matched");
#endif
            return *(T2<U>*)(obj->object);
        };

        template<typename U = T,
        typename std::enable_if<is_number<T1<U>>::value>::type* = nullptr>
        static void to(lua_State* L, T value) {
            lua_pushnumber(L, value);
        }

        template<typename U = T,
        typename std::enable_if<std::is_same<T1<U>, char const*>::value>::type* = nullptr>
        static void to(lua_State* L, char const* str) {
            lua_pushstring(L, str);
        };
        
        template<typename U = T,
        typename std::enable_if<std::is_same<T1<U>, ref>::value>::type* = nullptr>
        static void to(lua_State* L, ref const& r) {
            r.push(L);
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
        !std::is_pointer<T1<U>>::value && std::is_reference<U>::value>::type* = nullptr>
        static void to(lua_State* L, T&& value) {
            typedef T2<U> R;
            using tag_t = typename std::conditional<std::is_base_of<referenced_count, R>::value,
                referenced_count_tag, pure_pointer_tag>::type;
            // state ensures its existence
            lua_getfield(L, LUA_REGISTRYINDEX, "__objlink");
            lua_pushlightuserdata(L, &value);
            lua_rawget(L, -2);
            if (lua_isnoneornil(L, -1)) {
                lua_pop(L, 1); // pop the nil
                auto* wrapper = (object_wrapper*)lua_newuserdata(L, sizeof(object_wrapper));
                wrapper->object = &value;
                wrapper->type = &class_<T2<U>>::type();

                // FIXME: we don't really bother the lifetime for this for now
                // unless it's a referenced_count object
                object_meta<tag_t>::retain(&value);
                object_meta<tag_t>::push_metatable(L);
                lua_setmetatable(L, -2);
                lua_pushlightuserdata(L, &value);   // # tbl, value, key
                lua_pushvalue(L, -2);               // # tbl, value, key, value
                lua_rawset(L, -4);                  // # tbl, value
            }
            
            // remove the table, the object is already on the stack
            lua_remove(L, -2);
        }
        
        template<typename U = T,
        typename std::enable_if<is_userdata<T1<U>>::value &&
        !std::is_pointer<T1<U>>::value && !std::is_reference<U>::value>::type* = nullptr>
        static void to(lua_State* L, T&& value) {
            typedef typename std::remove_cv<U>::type R;
            // state ensures its existence
            lua_getfield(L, LUA_REGISTRYINDEX, "__objlink");
            lua_pushlightuserdata(L, &value);
            lua_rawget(L, -2);
            if (lua_isnoneornil(L, -1)) {
                lua_pop(L, 1); // pop the nil
                auto* wrapper = (object_wrapper*)lua_newuserdata(L, sizeof(object_wrapper));
                // TODO: constructor
                wrapper->object = new R(std::forward<T>(value));
                wrapper->type = &class_<T2<U>>::type();
                
                object_meta<raw_pointer_tag>::push_metatable(L, std::default_delete<R>());
                lua_setmetatable(L, -2);
                lua_pushlightuserdata(L, &value);   // # tbl, value, key
                lua_pushvalue(L, -2);               // # tbl, value, key, value
                lua_rawset(L, -4);                  // # tbl, value
            }
            
            // remove the table, the object is already on the stack
            lua_remove(L, -2);
        }
        
        template<typename U = T,
        typename std::enable_if<is_userdata<T2<U>>::value &&
        std::is_pointer<T1<U>>::value>::type* = nullptr>
        static void to(lua_State* L, T1<U> value) {
            typedef T2<U> R;
            using tag_t = typename std::conditional<std::is_base_of<referenced_count, R>::value,
                referenced_count_tag, pure_pointer_tag>::type;
            if (value == nullptr) {
                lua_pushnil(L);
                return;
            }

            // state ensures its existence
            lua_getfield(L, LUA_REGISTRYINDEX, "__objlink");
            lua_pushlightuserdata(L, value);
            lua_rawget(L, -2);
            
            if (lua_isnoneornil(L, -1)) {
                lua_pop(L, 1); // pop the nil
                auto* wrapper = (object_wrapper*)lua_newuserdata(L, sizeof(object_wrapper));
                wrapper->object = value;
                wrapper->type = &class_<R>::type();
                
                // we take the ownership as a regular pointer if it is a referenced count
                // otherwise, we ignore it for now
                object_meta<tag_t>::retain(value);
                object_meta<tag_t>::template push_metatable<R>(L, std::default_delete<R>());
                lua_setmetatable(L, -2);
                lua_pushlightuserdata(L, value); // # tbl, value, key
                lua_pushvalue(L, -2); // # tbl, value, key, value
                lua_rawset(L, -4); // # tbl, value
            }
            
            // remove the table, the object is already on the stack
            lua_remove(L, -2);
        }
    };
    
    template<class E, size_t _S>
    struct converter<std::array<E, _S>> {
        using R = std::array<E, _S>;
        static R from(lua_State* L, int idx, char* storage) {
            luaL_argcheck(L, lua_istable(L, idx), idx, "expect a table");
            
            R result;
            for (int i = 1;i <= _S; ++i) {
                lua_rawgeti(L, idx, i);
                if (lua_isnoneornil(L, -1)) {
                    lua_pop(L, 1);
                    break;
                }
                // E will never be a reference
                result[i-1] = converter<E>::from(L, -1, nullptr);
                lua_pop(L, 1);
            }
            return result;
        };
        
        static void to(lua_State* L, R && value) {
            lua_newtable(L);
            int i = 0;
            for (auto& it : value) {
                converter<E>::to(L, std::move(it));
                lua_rawseti(L, -2, ++i);
            }
        }
        
    };
    
    template<class E, size_t _S>
    struct converter<std::array<E, _S> const> : public converter<std::array<E, _S>> {};

    template<class E, size_t _S>
    struct converter<std::array<E, _S> const&> : public converter<std::array<E, _S>> {};
}

#endif