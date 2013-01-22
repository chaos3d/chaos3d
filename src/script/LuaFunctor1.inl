////////////////
// c function
template<class R, class A1, R (*TFnPtr)(A1)>
class CHAOS_API ApplierRA1 : public LuaFunctor{
public:
	typedef		R (*TFnPtr)(A1);
	typedef		ApplierRA1<R,A1> SELF;

	static int lua_call(lua_State* _L){
		typedef typename TypeTraits<typename TypeUnwind<R>::Type>::Type WrapType;
		typedef typename TypeDecider<typename TypeUnwind<R>::Type>::TYPE	RetType;

		ScriptState::PushUnwind<R>::push(
			L,
			(TFnPtr)(
				ScriptState::GetUnwind<A1>::get( L, 1 )
			),
			TYPE_FROM_NAME( WrapType )
			);
		return 1;
	}

	virtual std::string	description(){
		return typeName<R>() + "(*)("
			+ typeName<A1>() + ')';
	};
};

template<class A1>
class CHAOS_API ApplierRA1<void, A1> : public LuaFunctor{
protected:
	typedef		void (*TFnPtr)(A1);
	typedef		ApplierRA1<void,A1> SELF;
	TFnPtr		mPtr;

public:
	ApplierRA1( TFnPtr ptr ): LuaFunctor(SELF::lua_call), mPtr(ptr){
	};

#if defined(__LUA_CALL)
	static int lua_call(lua_State* _L){
		ScriptState L(_L);
		SELF*		thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));
#else
	virtual	void call(ScriptState const& L){
#endif
		(thiz->mPtr)(
			ScriptState::GetUnwind<A1>::get( L, 1 )
		);
#if defined(__LUA_CALL)
		return ScriptManager::getInstance()->return_(0);
#endif
	}

	virtual std::string	description(){
		return typeName<void>() + "(*)("
			+ typeName<A1>() + ')';
	};
};

////////////////
// class
template<class C,class R, class A1>
class CHAOS_API ApplierCRA1 : public LuaFunctor{
protected:
	typedef		R (C::*TFnPtr)(A1);
	typedef		ApplierCRA1<C,R,A1> SELF;
	TFnPtr		mPtr;

public:
	ApplierCRA1( TFnPtr ptr ): LuaFunctor(SELF::lua_call), mPtr(ptr){
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
					ScriptState::GetUnwind<A1>::get( L, 1 )
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
			+ typeName<C>() + "::*)("
			+ typeName<A1>() + ')';
	};

};

template<class C, class A1>
class CHAOS_API ApplierCRA1<C, void, A1> : public LuaFunctor{
protected:
	typedef		void (C::*TFnPtr)(A1);
	typedef		ApplierCRA1<C,void,A1> SELF;
	TFnPtr		mPtr;

public:
	ApplierCRA1( TFnPtr ptr ): LuaFunctor(SELF::lua_call), mPtr(ptr){
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
				ScriptState::GetUnwind<A1>::get( L, 1 )
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
			+ typeName<A1>() +')';
	};
};
