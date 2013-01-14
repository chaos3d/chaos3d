////////////////
// accessor
template<class T>
class CHAOS_API LuaGetter : public LuaFunctor{
protected:
	typedef LuaGetter<T> SELF;
	size_t	offset;

public:
	LuaGetter( size_t off ): LuaFunctor(SELF::lua_call), offset(off){
	};

#if defined(__LUA_CALL)
	static int lua_call(lua_State* _L){
		ScriptState L(_L);
		SELF*		thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));
		char* obj((char*)ScriptState::GetUnwind<void*>::get( L, lua_upvalueindex(1)));
		T*const&	address = (T*)(obj + thiz->offset);
#else
	virtual	void call(ScriptState const& L){
#endif
		typedef typename TypeTraits<typename TypeUnwind<T>::Type>::Type WrapType;

		ScriptState::PushUnwind<T>::push(
			L,
			*address,
			TYPE_FROM_NAME( WrapType )
			);
#if defined(__LUA_CALL)
		return 1;
#endif
	}

	virtual std::string	description(){
		return std::string("property getter(") + typeName<T>() + ')';
	};
};

template<class T>
class CHAOS_API LuaArrayGetter : public LuaFunctor{
protected:
	typedef LuaArrayGetter<T> SELF;
	size_t	offset;
	int		range;

public:
	LuaArrayGetter(size_t off, int r): LuaFunctor(SELF::lua_call), offset(off), range(r){
	};

#if defined(__LUA_CALL)
	static int lua_call(lua_State* _L){
		ScriptState L(_L);
		SELF*		thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));
		char* obj((char*)ScriptState::GetUnwind<void*>::get(L, lua_upvalueindex(1)));
		T*const&	address = (T*)(obj + thiz->offset);
		int const&	range(thiz->range);
#else
	virtual	void call(ScriptState const& L){
#endif
		typedef typename TypeTraits<typename TypeUnwind<T>::Type>::Type WrapType;

		int num = lua_gettop(_L);
		for(int i=1;i<=num;++i){
			int idx = lua_tointeger(_L,i);
			if( idx < 0 || idx >= range )
				lua_pushnil(_L);	// silently ignore error
			else
				ScriptState::PushUnwind<T>::push(
						L,
						*(address+idx),
						TYPE_FROM_NAME( WrapType )
						);
		}
#if defined(__LUA_CALL)
		return num;
#endif
	}

	virtual std::string	description(){
		return std::string("property[] getter(") + typeName<T>() + ')';
	};
};

template<class T>
class CHAOS_API LuaSetter : public LuaFunctor{
protected:
	typedef LuaSetter<T> SELF;
	size_t	offset;

public:
	LuaSetter(size_t off): LuaFunctor(SELF::lua_call), offset(off){
	};

#if defined(__LUA_CALL)
	static int lua_call(lua_State* _L){
		ScriptState L(_L);
		SELF*		thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));
		char* obj((char*)ScriptState::GetUnwind<void*>::get( L, lua_upvalueindex(1)));
		T*const&	address = (T*)(obj + thiz->offset);
#else
	virtual	void call(ScriptState const& L){
#endif
		typedef typename TypeTraits<typename TypeUnwind<T>::Type>::Type WrapType;

		*address = ScriptState::GetUnwind<T>::get(L, 1);
#if defined(__LUA_CALL)
		lua_pushvalue(_L,lua_upvalueindex(1));	// to stream coding, obj.set().set()
		return 1;
#endif
	}

	virtual std::string	description(){
		return std::string("property getter(") + typeName<T>() + ')';
	};
};

template<class T>
class CHAOS_API LuaArraySetter : public LuaFunctor{
protected:
	typedef LuaArraySetter<T> SELF;
	size_t	offset;
	int		range;

public:
	LuaArraySetter(size_t off,int r): LuaFunctor(SELF::lua_call), offset(off), range(r){
	};

#if defined(__LUA_CALL)
	static int lua_call(lua_State* _L){
		ScriptState L(_L);
		SELF*		thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));
		char* obj((char*)ScriptState::GetUnwind<void*>::get( L, lua_upvalueindex(1)));
		T*const&	address = (T*)(obj + thiz->offset);
		int const&	range(thiz->range);
#else
	virtual	void call(ScriptState const& L){
#endif
		typedef typename TypeTraits<typename TypeUnwind<T>::Type>::Type WrapType;

		int idx = lua_tointeger(_L,1);
		int num = idx + lua_gettop(_L);	// obj.set(idx, v1, v2, ..)
		num = num > range ? range: num;
		for(int i=idx;i<num;++i){
			*(address+i) = ScriptState::GetUnwind<T>::get(L,i-idx+2);
		}
#if defined(__LUA_CALL)
		lua_pushvalue(_L,lua_upvalueindex(1));	// to stream coding, obj.set().set()
		return 1;
#endif
	}

	virtual std::string	description(){
		return std::string("property[] setter(") + typeName<T>() + ')';
	};
};

