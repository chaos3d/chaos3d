#ifndef _UTILITY_H
#define _UTILITY_H

#include <type_traits>
#include <memory>
#include <cfloat>

// one-liner that the function returns the pointer and it will be only used once
//  it is wrapped by a unique_ptr that it'll be released in its destructor
#define WRAP_PTR(exp) std::unique_ptr<std::remove_pointer<decltype(exp)>::type>(exp).get()

#define ATTRIBUTE(type, name, val) \
    private: type _##name = (val); \
    public: type const& name () const { return _##name; } \
    public: type & name () { return _##name; } \
public: auto set_##name(type const& name) -> decltype(*this) \
    { _##name = name; return *this; } \
    public: template<class... Types> \
    auto set_##name(Types&&... args) -> decltype(*this) { \
        _##name = type(std::forward<Types>(args)...); \
        return *this; \
    }

#define DEFINE_LOADER   template<class Loader> \
                        std::pair<bool, typename Loader::position_t> load_from(Loader const&)
#define SPECIFY_LOADER_FOR(type, Loader)    template<> \
                                            std::pair<bool, Loader::position_t> type::load_from<Loader>(Loader const& loader)

#ifndef M_PI
#define	M_PI		(3.1415926)
#endif

#ifndef HALF_PI
#define	HALF_PI		(M_PI/2.0)
#endif

#define DEGREE_TO_RADIAN(degree)	((degree)/180.0*M_PI)
#define RADIAN_TO_DEGREE(radian)	((radian)/M_PI*180.0)

template<class T, class... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// move/copy ptr detector
// it helps to detect how to initialize pointers (unique/shared, move/copy)
template<class P, class C = void> struct init_ptr;

template<class P>
struct init_ptr<P, typename std::enable_if<!std::is_copy_constructible<P>::value>::type> {
    typedef P && type;
};

template<class P>
struct init_ptr<P, typename std::enable_if<std::is_copy_constructible<P>::value>::type> {
    typedef P const& type;
};

// unpacking to a list of numbers...
// usage:
//template<typename... Args>
//void callFunc(Args... args) {
//    func(std::get<typename tuple_gens<sizeof...(Args)>::type()>(args) ...);
//}
template<int ...> struct tuple_seq { };
template<int N, int ...S> struct tuple_gens : tuple_gens<N-1, N-1, S...> { };
template<int ...S> struct tuple_gens<0, S...> { typedef tuple_seq<S...> type; };

// get the index of a type in a tuplle
template<int Index, class Search, class First, class... Types>
struct tuple_get_index
{
    typedef typename tuple_get_index<Index + 1, Search, Types...>::type type;
    static constexpr int index = Index;
};

template<int Index, class Search, class... Types>
struct tuple_get_index<Index, Search, Search, Types...>
{
    typedef tuple_get_index type;
    static constexpr int index = Index;
};

template<class T, class... Types>
T tuple_get(std::tuple<Types...> const& tuple)
{
    return std::get<tuple_get_index<0,T,Types...>::type::index>(tuple);
}
#endif