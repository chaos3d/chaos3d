///----------------------------------------------------------------
///----------------------------------------------------------------
/// General type unwind
template<class T>
struct ScriptState::GetUnwind{
	// return const reference to avoid copy constructor
	inline static T const& get(ScriptState const& L, int index){
		return *L.getValue<T>(index);
	}
};

template<class T>
struct ScriptState::GetUnwind<T const>{
	// return const reference to avoid copy constructor
	inline static T const& get(ScriptState const& L, int index){
		return *L.getValue<T>(index);
	}
};

template<class T>
struct ScriptState::GetUnwind<T *>{
	inline static T * get(ScriptState const& L, int index){
		return (T*)L.getObject(index);	
	}
};

template<class T>
struct ScriptState::GetUnwind<T const*>{
	inline static T const* get(ScriptState const& L, int index){
		return (T*)L.getObject(index);	
	}
};

template<class T>
struct ScriptState::GetUnwind<T *const>{
	inline static T *const get(ScriptState const& L, int index){
		return (T*)L.getObject(index);	
	}
};

template<class T>
struct ScriptState::GetUnwind<T const*const>{
	inline static T const*const get(ScriptState const& L, int index){
		return (T*)L.getObject(index);	
	}
};

template<class T>
struct ScriptState::GetUnwind<T &>{
	inline static T & get(ScriptState const& L, int index){
		return *L.getValue<T>(index);	
	}
};

template<class T>
struct ScriptState::GetUnwind<T const&>{
	inline static T const& get(ScriptState const& L, int index){
		return *L.getValue<T>(index);	
	}
};

template<class T>
struct ScriptState::GetUnwind<T *const&>{
	inline static T *const& get(ScriptState const& L, int index){
		return (T *)L.getObject(index);	
	}
};

template<class T>
struct ScriptState::GetUnwind<T const*const&>{
	inline static T const*const& get(ScriptState const& L, int index){
		return (T*)L.getObject(index);	
	}
};

// doesn't support native type pointer or pointer reference
// because lua doesn't
// but push doesn't matter
template<class T> struct ScriptState::GetUnwind<T *&>{};
template<class T> struct ScriptState::GetUnwind<T const*&>{};


///----------------------------------------------------------------
template<class T>
struct ScriptState::PushUnwind{
	// there is an implicit cast from T to type of val
	// each specification should conform this cast.
	//
	// to avoid copy constructor
	inline static void push(ScriptState const& L, T const& val, Type* type){
		typedef typename TypeDecider<typename TypeUnwind<T>::Type *>::TYPE	GCType;\
		L.pushValue<GCType>( (GCType)new T(val), type, true);
	}
};

template<class T>
struct ScriptState::PushUnwind<T const>{
	inline static void push(ScriptState const& L, T const& val, Type* type){
		typedef typename TypeDecider<typename TypeUnwind<T>::Type *>::TYPE	GCType;\
		L.pushValue<GCType>( (GCType)new T(val), type, true);
	}
};

template<class T>
struct ScriptState::PushUnwind<T &>{
	inline static void push(ScriptState const& L, T& val, Type* type){
		typedef typename TypeDecider<typename TypeUnwind<T>::Type *>::TYPE	GCType;\
		L.pushValue<GCType>( (GCType)&val, type, false);
	}
};

template<class T>
struct ScriptState::PushUnwind<T const&>{
	inline static void push(ScriptState const& L, T const& val, Type* type){
		typedef typename TypeDecider<typename TypeUnwind<T>::Type *>::TYPE	GCType;\
		L.pushValue<GCType>( (GCType)new T(val), type, true);
	}
};

template<class T>
struct ScriptState::PushUnwind<T *>{
	inline static void push(ScriptState const& L, T* val, Type* type){
		typedef typename TypeDecider<typename TypeUnwind<T>::Type *>::TYPE	GCType;\
		L.pushValue<GCType>( (GCType)val, type, false);
	}
};

template<class T>
struct ScriptState::PushUnwind<T const*>{
	inline static void push(ScriptState const& L, T const* val, Type* type){
		typedef typename TypeDecider<typename TypeUnwind<T>::Type *>::TYPE	GCType;\
		L.pushValue<GCType>( (GCType)new T(*val), type, true);
	}
};

template<class T>
struct ScriptState::PushUnwind<T *const>{
	inline static void push(ScriptState const& L, T *const val, Type* type){
		typedef typename TypeDecider<typename TypeUnwind<T>::Type *>::TYPE	GCType;\
		L.pushValue<GCType>( (GCType)val, type, false);
	}
};

template<class T>
struct ScriptState::PushUnwind<T const*const>{
	inline static void push(ScriptState const& L, T const*const val, Type* type){
		typedef typename TypeDecider<typename TypeUnwind<T>::Type *>::TYPE	GCType;\
		L.pushValue<GCType>( (GCType)new T(val), type, true);
	}
};

template<class T>
struct ScriptState::PushUnwind<T *&>{
	inline static void push(ScriptState const& L, T*& val, Type* type){
		typedef typename TypeDecider<typename TypeUnwind<T>::Type *>::TYPE	GCType;\
		L.pushValue<GCType>( (GCType)val, type, false);
	}
};

template<class T>
struct ScriptState::PushUnwind<T const*&>{
	inline static void push(ScriptState const& L, T const*& val, Type* type){
		typedef typename TypeDecider<typename TypeUnwind<T>::Type *>::TYPE	GCType;\
		L.pushValue<GCType>( (GCType)new T(val), type, true);
	}
};

template<class T>
struct ScriptState::PushUnwind<T * const&>{
	inline static void push(ScriptState const& L, T* const& val, Type* type){
		typedef typename TypeDecider<typename TypeUnwind<T>::Type *>::TYPE	GCType;\
		L.pushValue<GCType>( (GCType)val, type, false);
	}
};

template<class T>
struct ScriptState::PushUnwind<T const* const&>{
	inline static void push(ScriptState const& L, T const* const& val, Type* type){
		typedef typename TypeDecider<typename TypeUnwind<T>::Type *>::TYPE	GCType;\
		L.pushValue<GCType>( (GCType)new T(val), type, true);
	}
};


///----------------------------------------------------------------
// * remember to include ScriptState.h
#define	TYPE_RET_DYN(T)	\
template<>\
struct ScriptState::PushUnwind<T&>{\
	inline static void push(ScriptState const& L, T const& val, Type* type){\
		typedef TypeDecider<TypeUnwind<T>::Type *>::TYPE	GCType;			\
		L.pushValue<GCType>( (GCType)&val, val.getClassType(), false);\
	}\
};\
\
template<>\
struct ScriptState::PushUnwind<T *>{\
	inline static void push(ScriptState const& L, T *val, Type* type){\
		typedef TypeDecider<TypeUnwind<T>::Type *>::TYPE	GCType;			\
		L.pushValue<GCType>( (GCType)val, val == 0 ? type : val->getClassType(), false);\
	}\
};\
\
template<>\
struct ScriptState::PushUnwind<T *const>{\
	inline static void push(ScriptState const& L, T *const val, Type* type){\
		typedef TypeDecider<TypeUnwind<T>::Type *>::TYPE	GCType;			\
		L.pushValue<GCType>( (GCType)val, val == 0 ? type : val->getClassType(), false);\
	}\
};\
\
template<>\
struct ScriptState::PushUnwind<T *&>{\
	inline static void push(ScriptState const& L, T*& val, Type* type){\
		typedef TypeDecider<TypeUnwind<T>::Type *>::TYPE	GCType;			\
		L.pushValue<GCType>( (GCType)val, val == 0 ? type : val->getClassType(), false);\
	}\
};\
\
template<>\
struct ScriptState::PushUnwind<T * const&>{\
	inline static void push(ScriptState const& L, T* const& val, Type* type){\
		typedef TypeDecider<TypeUnwind<T>::Type *>::TYPE	GCType;			\
		L.pushValue<GCType>( (GCType)val, val == 0 ? type : val->getClassType(), false);\
	}\
};

/*
template<> struct TypeDecider<T*>{	typedef	T* TYPE; }; \
template<> struct ScriptState::PushUnwind<T>{\
	inline void operator()(ScriptState const& L, T const& val, Type*){\
		L.pushValue<void>( new T(val), val.getClassType(), true);\
	}\
};\
\
template<> struct ScriptState::PushUnwind<T &>{\
	inline void operator()(ScriptState const& L, T& val, Type*){\
		L.pushValue<void>( &val, val.getClassType(), false);\
	}\
};\
\
template<> struct ScriptState::PushUnwind<T *>{\
	inline void operator()(ScriptState const& L, T* val, Type* type){\
		L.pushValue<void>( val, val == 0 ? type : val->getClassType(), false);\
	}\
};\
\
template<> struct ScriptState::PushUnwind<T *&>{\
	inline void operator()(ScriptState const& L, T*& val, Type* type){\
		L.pushValue<void>( val, val == 0 ? type : val->getClassType(), false);\
	}\
};\
\
template<> struct ScriptState::PushUnwind<T * const&>{\
	inline void operator()(ScriptState const& L, T* const& val, Type* type){\
		L.pushValue<void>( val, val == 0 ? type : val->getClassType(), false);\
	}\
};
*/
/*
template<class T>
struct ScriptState::PushUnwind<T const*>{
	inline void operator()(ScriptState const& L, T* const val, Type* type){
		// because lua has no const qualifier, in whatever case
		// the const will be removed
		k=1;
		L.pushValue<T>( const_cast<T*>(val), type, false);
	}
};
template<class T>
struct ScriptState::PushUnwind<T * const&>{
	inline void operator()(ScriptState const& L, T* const&val, Type* type){
		L.pushValue<T>( const_cast<T*>(val), type, false);
	}
};
*/

/*
template<>	struct ScriptState::GetUnwind<char>;
template<>	struct ScriptState::GetUnwind<short>;
template<>	struct ScriptState::GetUnwind<int>;
template<>	struct ScriptState::GetUnwind<long>;
template<>	struct ScriptState::GetUnwind<float>;
template<>	struct ScriptState::GetUnwind<double>;
*/

///----------------------------------------------------------------
///----------------------------------------------------------------
/// Native type cast
#define GET_NATIVE_NUMCAST(type)										\
		template<>														\
		inline type ScriptState::getNativeValue<type>(int index) const{	\
			return (type)lua_tonumber( mL, index );						\
		};

#define PUSH_NATIVE_NUMCAST(type)										\
		template<>														\
		inline void	ScriptState::pushNativeValue<type>(type val) const{	\
			lua_pushnumber( mL, (lua_Number)val);									\
		};

#define PUSH_NATIVE_STRCAST(type)										\
		template<>														\
		inline void ScriptState::pushNativeValue<type>(type val) const{	\
			return lua_pushstring( mL, val );							\
		};

#define PUSH_NATIVE_STDSTRCAST(type)										\
		template<>													\
		inline void ScriptState::pushNativeValue<type>(type val) const{	\
			return lua_pushstring( mL, val.c_str() );				\
		};

#define GET_NATIVE_STDSTRCAST(type)										\
		template<>													\
		inline type ScriptState::getNativeValue<type>(int index) const{	\
			return std::string(lua_tostring( mL, index ));				\
		};

GET_NATIVE_NUMCAST(char);
GET_NATIVE_NUMCAST(int);
GET_NATIVE_NUMCAST(short);
GET_NATIVE_NUMCAST(long);
GET_NATIVE_NUMCAST(size_t);
GET_NATIVE_NUMCAST(float);
GET_NATIVE_NUMCAST(double);
GET_NATIVE_STDSTRCAST(std::string);

PUSH_NATIVE_NUMCAST(char);
PUSH_NATIVE_NUMCAST(int);
PUSH_NATIVE_NUMCAST(short);
PUSH_NATIVE_NUMCAST(long);
PUSH_NATIVE_NUMCAST(size_t);
PUSH_NATIVE_NUMCAST(float);
PUSH_NATIVE_NUMCAST(double);

/// string cast
PUSH_NATIVE_STRCAST(char const*);
PUSH_NATIVE_STRCAST(char const*&);
PUSH_NATIVE_STDSTRCAST(std::string const&);

template<>
inline char const* ScriptState::getNativeValue<char const*>(int index) const{
	return lua_tostring( mL, index );
}

template<>
inline char const* const ScriptState::getNativeValue<char const* const>(int index) const{
	return lua_tostring( mL, index );
}

// lua cast
template<>
inline void ScriptState::pushNativeValue<TypeLua>(TypeLua val) const{
	return val.get(mL);
};

template<>
inline void ScriptState::pushNativeValue<TypeLua const&>(TypeLua const& val) const{
	return val.get(mL);
};

template<>
inline TypeLua ScriptState::getNativeValue<TypeLua>(int index) const{
	lua_pushvalue( mL, index );
	return TypeLua( lua_ref(mL,1) );
}

// boolean
template<>
inline void ScriptState::pushNativeValue<bool>(bool val) const{
	return lua_pushboolean( mL, val );
};

template<>
inline bool ScriptState::getNativeValue<bool>(int index) const{
	return lua_toboolean( mL, index ) == 1;
}

// ref count pointer
template<>
inline void ScriptState::pushNativeValue<ReferencedPtr>(ReferencedPtr val) const{
	pushValue<ReferencedCount*>(val.getPointer(), 0, true);
};

// @see ScriptState.cpp
template<>
ReferencedPtr ScriptState::getNativeValue<ReferencedPtr>(int index) const;

template<>
ReferencedPtr ScriptState::getNativeValue<ReferencedPtr>(int index) const;

///----------------------------------------------------------------
///----------------------------------------------------------------
/// Native type unwind

// don't unwind const& of TypeLua
/*
template<>	
struct ScriptState::GetUnwind<TypeLua const&>{
	inline TypeLua const& operator()(ScriptState const& L, int index){
		return L.getNativeValue<TypeLua>(index);
	};
};

template<>	
struct ScriptState::PushUnwind<TypeLua const&>{
	inline void operator()(ScriptState const& L, TypeLua const& val, Type*){
		return L.pushNativeValue<TypeLua const&>(val);
	};
};
*/

#define GET_UNWIND_NATIVE(type)											\
		template<>														\
		struct ScriptState::GetUnwind<type>{							\
			inline static type get(ScriptState const& L, int index){	\
				return L.getNativeValue<type>(index);					\
			};															\
		};

#define PUSH_UNWIND_NATIVE(type)										\
		template<>														\
		struct ScriptState::PushUnwind<type>{							\
			inline static void push(ScriptState const& L, type val, Type*){	\
				return L.pushNativeValue<type>(val);					\
			}															\
		};

GET_UNWIND_NATIVE(char);
GET_UNWIND_NATIVE(bool);
GET_UNWIND_NATIVE(short);
GET_UNWIND_NATIVE(int);
GET_UNWIND_NATIVE(long);
GET_UNWIND_NATIVE(size_t);
GET_UNWIND_NATIVE(float);
GET_UNWIND_NATIVE(double);
GET_UNWIND_NATIVE(std::string);
GET_UNWIND_NATIVE(TypeLua);

template<>
struct ScriptState::GetUnwind<TypeLua const&>{
	inline static TypeLua get(ScriptState const& L, int index){
		return L.getNativeValue<TypeLua>(index);
	};
};

/*
GET_UNWIND_NATIVE(ReferencedPtr);

template<class T>
struct ScriptState::GetUnwind<ReferencedPtrT<T> >{
	inline ReferencedCount operator()(ScriptState const& L, int index){
		return L.getNativeValue<ReferencedCount>(index);
	};
};
*/

PUSH_UNWIND_NATIVE(char);
PUSH_UNWIND_NATIVE(bool);
PUSH_UNWIND_NATIVE(short);
PUSH_UNWIND_NATIVE(int);
PUSH_UNWIND_NATIVE(long);
PUSH_UNWIND_NATIVE(size_t);
PUSH_UNWIND_NATIVE(float);
PUSH_UNWIND_NATIVE(double);
PUSH_UNWIND_NATIVE(char const*);
PUSH_UNWIND_NATIVE(char const*&);
PUSH_UNWIND_NATIVE(TypeLua);
PUSH_UNWIND_NATIVE(TypeLua const&);

#define PUSH_UNWIND_STDSTRING(type)										\
		template<>														\
		struct ScriptState::PushUnwind<type>{							\
			inline static void push(ScriptState const& L, std::string const& val, Type*){	\
				return L.pushNativeValue<std::string const&>(val);		\
			}															\
		};
PUSH_UNWIND_STDSTRING(std::string);
PUSH_UNWIND_STDSTRING(std::string const);
PUSH_UNWIND_STDSTRING(std::string &);
PUSH_UNWIND_STDSTRING(std::string const&);
/*
template<>
struct ScriptState::PushUnwind<std::string>{
	inline static void push(ScriptState const& L, std::string const& val, Type*){
		return L.pushNativeValue<std::string const&>(val);
	}
};

template<>
struct ScriptState::PushUnwind<std::string const&>{
	inline static void push(ScriptState const& L, std::string const& val, Type*){
		return L.pushNativeValue<std::string const&>(val);
	}
};

template<>
struct ScriptState::PushUnwind<std::string&>{
	inline static void push(ScriptState const& L, std::string const& val, Type*){
		return L.pushNativeValue<std::string const&>(val);
	}
};
*/
///----------------------------------------------------------------
///----------------------------------------------------------------
/// disable native types pointer
#define DISABLE_GET_UNWIND(type)	\
		template<>	struct ScriptState::GetUnwind<type*>{};			\
		template<>	struct ScriptState::GetUnwind<type&>{};			\
		template<>	struct ScriptState::GetUnwind<type*&>{};			
//		template<>	struct ScriptState::GetUnwind<type* const>{};	\
//		template<>	struct ScriptState::GetUnwind<type const*>{};	

DISABLE_GET_UNWIND(bool);
DISABLE_GET_UNWIND(short);
DISABLE_GET_UNWIND(int);
DISABLE_GET_UNWIND(long);
DISABLE_GET_UNWIND(size_t);
DISABLE_GET_UNWIND(float);
DISABLE_GET_UNWIND(double);
DISABLE_GET_UNWIND(TypeLua);
DISABLE_GET_UNWIND(std::string);

///----------------------------------------------------------------
/// doesn't unwind char type
template<> struct ScriptState::GetUnwind<char*>{};			// only for const chars
template<> struct ScriptState::GetUnwind<char* const>{};	// and const chars pointer

template<> 
struct ScriptState::GetUnwind<char const*>{
	inline static char const* get(ScriptState const& L, int index){
		return L.getNativeValue<char const*>(index);
	}
};

template<> 
struct ScriptState::GetUnwind<char const* const>{
	inline static char const* const get(ScriptState const& L, int index){
		return L.getNativeValue<char const*>(index);
	}
};

///----------------------------------------------------------------
/// unwind ReferencedCount not to void
template<>
struct ScriptState::PushUnwind<ReferencedCount*>{
	inline void operator()(ScriptState const& L, ReferencedCount* val, Type* type){
		L.pushValue<ReferencedCount*>( val, type, true);
	}
};

