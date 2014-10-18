#ifndef _CHAOS3D_SCRIPT_CLASS_TYPE_H
#define _CHAOS3D_SCRIPT_CLASS_TYPE_H

#include <algorithm>
#include <cassert>
#include <typeinfo>
#include <utility>
#include <liblua/lua/lua.hpp>
#include "script/type_info.h"
#include "common/log.h"

#define IMPORT_CLASS_TYPE(clz) \
    extern template class script::class_<clz>

namespace script {
    class state;
    
    template<class T>
    struct gc_policy;
    
    // type info for the object in Lua
    class type_info {
    public:
        typedef std::unordered_map<std::string, lua_function_t> meta_t;
        typedef std::vector<type_info const*> bases_t;
        
    public:
        // TODO: wrapper a type struct to do sanity check
        type_info& def(char const* name, lua_function_t func) {
            _meta.emplace(name, func);
            return *this;
        }
        
        bool is_derived(type_info const* base) const {
            for (auto& it : _bases) {
                if (it == base)
                    return true;
                else if (it->is_derived(base))
                    return true;
            }
            return false;
        }

#if 0
        virtual char const* name() const override {
            return _name.c_str();
        }
        
        virtual bases_t const& bases() const override {
            return _bases;
        }
        
        virtual meta_t const& meta() const override {
            return _meta;
        }
#endif

        lua_function_t find(std::string const& name) const {
            auto it = _meta.find(name);
            if (it == _meta.end()) {
                lua_function_t found = nullptr;
                for (auto& base : _bases) {
                    found = base->find(name);
                    if (found != nullptr)
                        return found;
                }
                return nullptr;
            } else {
                return it->second;
            }
        }
        
    protected:
        type_info() {
            //def("__gc", gc_policy<C>::gc);
            // TODO: some other meta function?
        }
        
        type_info(type_info const&) = delete;
        type_info& operator=(type_info const&) = delete;
        
        bases_t _bases;
        meta_t _meta;
        
        friend class state;
    };
    
    template<class C>
    class class_ : public type_info {
    public:
        typedef typename std::remove_reference<C>::type C0;
        typedef typename std::remove_cv<C0>::type C1;
        typedef typename std::remove_pointer<C1>::type C2;
        typedef typename std::remove_cv<C2>::type C3;
        typedef typename std::remove_pointer<C3>::type C4;
        static_assert(std::is_same<C4, C>::value, "define only raw type");
        
    public:
        template<class... Others, typename std::enable_if<sizeof...(Others) == 0>::type* = nullptr>
        class_& derive() {
            return *this;
        }
        
        template<class Base, class... Others>
        class_& derive() {
            static_assert(std::is_base_of<Base, C0>::value, "invalid base class");
            assert(std::find(_bases.begin(), _bases.end(), &class_<Base>::type())
                   == _bases.end());
            _bases.emplace_back(&class_<Base>::type());
            return derive<Others...>();
        }
        
        static class_& type() {
            static class_ _type;
            return _type;
        }
        
    protected:
        class_() {
            LOG_TRACE(state, "initialize type for: " << typeid(C).name()
                      << " at " << std::hex << (void*)this);
        }
    };
}
#endif