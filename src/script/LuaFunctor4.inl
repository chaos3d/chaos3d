////////////////
// c function
template<class R, class A1, class A2, class A3, class A4>
class CHAOS_API ApplierRA4 : public LuaFunctor{
protected:
	typedef		R (*TFnPtr)(A1,A2,A3,A4);
	typedef		ApplierRA4<R,A1,A2,A3,A4> SELF;
	TFnPtr		mPtr;

public:
	ApplierRA4( TFnPtr ptr ): LuaFunctor(SELF::lua_call), mPtr(ptr){
	};

#if defined(__LUA_CALL)
	static int lua_call(lua_State* _L){
		ScriptState L(_L);
		SELF*		thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));
#else
	virtual	void call(ScriptState const& L){
#endif
		typedef typename TypeTraits<typename TypeUnwind<R>::Type>::Type WrapType;
		typedef typename TypeDecider<typename TypeUnwind<R>::Type>::TYPE	RetType;


		ScriptState::PushUnwind<R>::push(
			L,
			((thiz->mPtr)(
				ScriptState::GetUnwind<A1>::get( L, 1 ),
				ScriptState::GetUnwind<A2>::get( L, 2 ),
				ScriptState::GetUnwind<A3>::get( L, 3 ),
				ScriptState::GetUnwind<A4>::get( L, 4 )
			)),
			TYPE_FROM_NAME( WrapType )
			);
#if defined(__LUA_CALL)
		return ScriptManager::getInstance()->return_(1);
#endif
	}

	virtual std::string	description(){
		return typeName<R>() + "(*)("
			+ typeName<A1>() + ','
			+ typeName<A2>() + ','
			+ typeName<A3>() + ','
			+ typeName<A4>() + ')';
	};
};

template<class A1, class A2, class A3, class A4>
class CHAOS_API ApplierRA4<void, A1, A2, A3, A4> : public LuaFunctor{
protected:
	typedef		void (*TFnPtr)(A1,A2,A3,A4);
	typedef		ApplierRA4<void,A1,A2,A3,A4> SELF;
	TFnPtr		mPtr;

public:
	ApplierRA4( TFnPtr ptr ): LuaFunctor(SELF::lua_call), mPtr(ptr){
	};

#if defined(__LUA_CALL)
	static int lua_call(lua_State* _L){
		ScriptState L(_L);
		SELF*		thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));
#else
	virtual	void call(ScriptState const& L){
#endif
		(thiz->mPtr)(
			ScriptState::GetUnwind<A1>::get( L, 1 ),
			ScriptState::GetUnwind<A2>::get( L, 2 ),
			ScriptState::GetUnwind<A3>::get( L, 3 ),
			ScriptState::GetUnwind<A4>::get( L, 4 )
		);
#if defined(__LUA_CALL)
		return ScriptManager::getInstance()->return_(0);
#endif
	}

	virtual std::string	description(){
		return typeName<void>() + "(*)("
			+ typeName<A1>() + ','
			+ typeName<A2>() + ','
			+ typeName<A3>() + ','
			+ typeName<A4>() + ')';
	};
};

////////////////
// class
template<class C, class R, class A1, class A2, class A3, class A4>
class CHAOS_API ApplierCRA4 : public LuaFunctor{
protected:
	typedef		R (C::*TFnPtr)(A1,A2,A3,A4);
	typedef		ApplierCRA4<C,R,A1,A2,A3,A4> SELF;
	TFnPtr		mPtr;

public:
	ApplierCRA4( TFnPtr ptr ): LuaFunctor(SELF::lua_call), mPtr(ptr){
	};

#if defined(__LUA_CALL)
	static int lua_call(lua_State* _L){
		ScriptState L(_L);
		SELF*		thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));
#else
	virtual	void call(ScriptState const& L){
#endif
		typedef typename TypeTraits<typename TypeUnwind<C>::Type>::Type CheckType;
		typedef typename TypeTraits<typename TypeUnwind<R>::Type>::Type WrapType;
		typedef typename TypeDecider<typename TypeUnwind<R>::Type>::TYPE	RetType;

		if( CHECK_TYPE( 
			L->checkType( 1, TYPE_FROM_NAME(CheckType)) 
			) )
		{
			 // only use void* to reuse template class
			C* obj = (C*)ScriptState::GetUnwind<void*>::get( L, lua_upvalueindex(1));

			ScriptState::PushUnwind<R>::push(
				L,
				((obj->*(thiz->mPtr))(
					ScriptState::GetUnwind<A1>::get( L, 1 ),
					ScriptState::GetUnwind<A2>::get( L, 2 ),
					ScriptState::GetUnwind<A3>::get( L, 3 ),
					ScriptState::GetUnwind<A4>::get( L, 4 )
				)),
				TYPE_FROM_NAME( WrapType )
				);
		}else{
			LOG("Object type doesn't match, uable to call methods. return nil.");
			lua_pushnil(L.getState());
		}
#if defined(__LUA_CALL)
		return ScriptManager::getInstance()->return_(1);
#endif
	}

	virtual std::string	description(){
		return typeName<R>() + '('
			+ typeName<C>() +	"::*)("
			+ typeName<A1>() + ','
			+ typeName<A2>() + ','
			+ typeName<A3>() + ','
			+ typeName<A4>() + ')';
	};
};

template<class C, class A1, class A2, class A3, class A4>
class CHAOS_API ApplierCRA4<C, void, A1, A2, A3, A4> : public LuaFunctor{
protected:
	typedef		void (C::*TFnPtr)(A1,A2,A3,A4);
	typedef		ApplierCRA4<C,void,A1,A2,A3,A4> SELF;
	TFnPtr		mPtr;

public:
	ApplierCRA4( TFnPtr ptr ): LuaFunctor(SELF::lua_call), mPtr(ptr){
	};

#if defined(__LUA_CALL)
	static int lua_call(lua_State* _L){
		ScriptState L(_L);
		SELF*		thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));
#else
	virtual	void call(ScriptState const& L){
#endif
		typedef typename TypeTraits<typename TypeUnwind<C>::Type>::Type CheckType;

		if( CHECK_TYPE( 
			L->checkType( 1, TYPE_FROM_NAME(CheckType)) 
			) )
		{
			C* obj = (C*)ScriptState::GetUnwind<void*>::get( L, lua_upvalueindex(1));

			((obj->*(thiz->mPtr))(
				ScriptState::GetUnwind<A1>::get( L, 1 ),
				ScriptState::GetUnwind<A2>::get( L, 2 ),
				ScriptState::GetUnwind<A3>::get( L, 3 ),
				ScriptState::GetUnwind<A4>::get( L, 4 )
			));
		}else{
			LOG("Object type doesn't match, uable to call methods. return nil.");
		}
#if defined(__LUA_CALL)
		return ScriptManager::getInstance()->return_(0);
#endif
	}

	virtual std::string	description(){
		return typeName<void>() + '('
			+ typeName<C>() + "::*)("
			+ typeName<A1>() + ','
			+ typeName<A2>() + ','
			+ typeName<A3>() + ','
			+ typeName<A4>() + ')';
	};
};
