#ifndef _CHAOS3D_SCRIPT_CLASS_TYPE_H
#define _CHAOS3D_SCRIPT_CLASS_TYPE_H

#include <typeinfo>
#include <utility>
#include <liblua/lua/lua.hpp>
#include "script/type_info.h"

namespace script {
    class state;
    
    template<class T>
    struct gc_policy;
    
    // load meta table
    
    template<class C>
    class class_ : public type_info{
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
            static_assert(std::is_base_of<C0, Base>::value, "invalid base class");
            _bases.emplace_back(&class_<Base>::type());
            return derive<Others...>();
        }
        
        // TODO: wrapper a type struct to do sanity check
        class_& def(char const* name, lua_function_t func) {
            _meta.emplace(name, func);
            return *this;
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

        virtual lua_function_t find(std::string const& name) const override {
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
        
        static class_<C>& type() {
            static class_<C> _type;
            return _type;
        }
        
    private:
        class_() {
            //def("__gc", gc_policy<C>::gc);
            // TODO: some other meta function?
        }
        
        class_(class_ const&) = delete;
        class_& operator=(class_ const&) = delete;
        
        bases_t _bases;
        meta_t _meta;
        
        friend class state;
    };
}
#endif