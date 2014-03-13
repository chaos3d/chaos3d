#ifndef _CHAOS3D_SCRIPT_STATE_H
#define _CHAOS3D_SCRIPT_STATE_H

#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>
#include <liblua/lua/lua.hpp>
#include "common/utility.h"
#include "script/class_type.h"
#include "script/lua_ref.h"
#include "script/import_scope.h"

class data_stream;

namespace script {
    class state;
    
    class coroutine {
    public:
        typedef std::shared_ptr<state> parent_ptr;
        
    public:
        ~coroutine();

        coroutine(coroutine&& rhs)
        : _L(rhs._L), _co_ref(std::move(rhs._co_ref)) {
            rhs._L = NULL;
        }
        
        coroutine& operator=(coroutine&& rhs) {
            _L = rhs._L;
            rhs._L = NULL;
            _co_ref = std::move(rhs._co_ref);
            return *this;
        }
        
        template<class... Rs, class... Args>
        std::tuple<bool, Rs...> resume(Args&&... args) {
            return resume_helper<Rs...>(typename tuple_gens<sizeof...(Rs)>::type(),
                                        std::forward<Args>(args)...);
        }
        
        // whether safe to call resume
        // it could be
        //  a) it's just created with the function at the bottome of the stack or
        //  b) it's being yielded
        bool is_yielded() const;
        
        // for other wrappers, do not use
        lua_State* internal() const { return _L; }
        parent_ptr const& parent() const { return _co_ref.parent(); }
        
    private:
        template<class... Rs, class... Args, int... N>
        std::tuple<bool, Rs...> resume_helper(tuple_seq<N...>, Args&&... args) {
            push(std::forward<Args>(args)...);
            if (!resume_with(sizeof...(Args)))
                return std::make_tuple(false, Rs()...);
            
            auto result = std::make_tuple(true, converter<Rs>::from(_L, N + 1, nullptr)...);
            lua_settop(_L, 0);
            return std::move(result);
        }
        
        bool resume_with(int nargs = 0);
        
        template<class Arg, class... Args>
        void push(Arg&& arg, Args&&... args) {
            converter<Arg>::to(_L, std::forward<Arg>(arg));
            return push(std::forward<Args>(args)...);
        }
        
        // termination
        void push() {};
        
        coroutine(coroutine const&) = delete;
        coroutine& operator=(coroutine const&) = delete;
        
        // create from the thread being on top of the state
        coroutine(state* parent);
        
        lua_State* _L;
        ref _co_ref;
        
        friend class state;
        friend struct deleter;
    };

    class state : public std::enable_shared_from_this<state> {
    public:
        typedef std::shared_ptr<state> ptr;
        typedef std::vector<coroutine> coroutines_t;
        typedef std::vector<int> coroutine_refs_t;
        
    public:
        ~state();
        
        coroutine load(char const*, char const* = nullptr);
        coroutine load(data_stream*, char const* = nullptr);
        
        // internal ref for other wrappers, do not use
        lua_State* internal() const { return _L; };
        
        import_scope import(char const* name = nullptr);
        
        template<class C>
        state& import(C&& value, char const* name, char const* scope = nullptr) {
            using C0 = typename std::remove_cv<C>::type;
            using C1 = typename std::remove_pointer<C0>::type;
            static_assert(!std::is_pointer<C1>::value, "does not support pointer of pointer type");
            static_assert(std::is_pointer<C0>::value && !std::is_const<C1>::value,
                          "does not support pointer of const value");

            push_scope(_L, scope);
            lua_pushstring(_L, name);
            converter<C>::to(_L, value);
            lua_rawset(_L, -3);
            lua_pop(_L, 1); // pop the scope table
            return *this;// import_internal<C0>(value, name, scope);
        }
        
    private:
        template<class C, typename std::enable_if<std::is_pointer<C>::value>::type* = nullptr>
        state& import_internal(C&& value, char const* name, char const* scope) {
            using C1 = typename std::remove_pointer<C>::type;
            return import((void*)value, name, &class_<C1>::type(), scope);
        }
        
        template<class C, typename std::enable_if<!std::is_pointer<C>::value>::type* = nullptr>
        state& import_internal(C&& value, char const* name, char const* scope) {
            using C1 = typename std::remove_cv<C>::type;
            return import(new C1(value), name, &class_<C1>::type(), scope);
        }
        
        state& import(void* data, char const*, type_info const*, char const* scope);
        
        // push the named scope(table) or the global table
        void push_scope(lua_State*, char const* scope);
        
        state(bool = true);
        
        // make sure the table to link all the objects are reference as 1
        void ensure_objlink();
        
        void recycle(coroutine &&);
        coroutine fetch();
        
        lua_State* _L;
        
        coroutines_t _coroutines;
        coroutine_refs_t _co_refs;
        
        CONSTRUCTOR_FOR_SHARED(state);
        
        friend class coroutine;
    };
}
#endif