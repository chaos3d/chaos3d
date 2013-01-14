////////////////
// obj-c function
class CHAOS_API ApplierOC : public LuaFunctor{
protected:
	SEL		mSelector;

public:
	ApplierOC(SEL sel, LuaFunc func):LuaFunctor(func), mSelector(sel){
	}
};

template<class R, class A1>
class CHAOS_API ApplierOCRA1: public ApplierOC{
public:
	typedef	ApplierOCRA1<R,A1> SELF;
	ApplierOCRA1(SEL sel):ApplierOC(sel,SELF::lua_call){
	};

	static int lua_call(lua_State* _L){
		typedef typename TypeTraits<typename TypeUnwind<R>::Type>::Type WrapType;
		typedef typename TypeDecider<typename TypeUnwind<R>::Type>::TYPE RetType;
		typedef R (*MsgSender)(id,SEL,A1);
		ScriptState L(_L);
		id obj = (id)ScriptState::GetUnwind<void*>::get( L, lua_upvalueindex(1));
		SELF* thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));


		ScriptState::PushUnwind<R>::push(
			L,
			((MsgSender)objc_msgSend)(obj,thiz->mSelector,
				ScriptState::GetUnwind<A1>::get( L, 1 )
			),
			TYPE_FROM_NAME( WrapType )
			);
		return ScriptManager::getInstance()->return_(1);
	}

	virtual std::string	description(){
		return typeName<R>() + "(*selector)("
			+ typeName<A1>() + ')';
	};
};

template<class A1>
class CHAOS_API ApplierOCRA1<void, A1> : public ApplierOC{
public:
	typedef		ApplierRA1<void,A1> SELF;
	ApplierOCRA1(SEL sel):ApplierOC(sel,SELF::lua_call){
	};

	static int lua_call(lua_State* _L){
		typedef void (*MsgSender)(id,SEL,A1);
		ScriptState L(_L);
		id obj = (id)ScriptState::GetUnwind<void*>::get(L, lua_upvalueindex(1));
		SELF* thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));

		((MsgSender)objc_msgSend)(obj,thiz->mSelector,
			ScriptState::GetUnwind<A1>::get( L, 1 )
		);
		return ScriptManager::getInstance()->return_(0);
	}

	virtual std::string	description(){
		return typeName<void>() + "(*selector)("
			+ typeName<A1>() + ')';
	};
};

#define SELECTOR1(sel, R, A1)	new ApplierOCRA1<R,A1>(sel)
