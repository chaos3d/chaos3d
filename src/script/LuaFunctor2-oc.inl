////////////////
// obj-c function
template<class R, class A1, class A2>
class CHAOS_API ApplierOCRA2: public ApplierOC{
public:
	typedef	ApplierOCRA2<R,A1,A2> SELF;
	ApplierOCRA2(SEL sel):ApplierOC(sel,SELF::lua_call){
	};

	static int lua_call(lua_State* _L){
		typedef typename TypeTraits<typename TypeUnwind<R>::Type>::Type WrapType;
		typedef typename TypeDecider<typename TypeUnwind<R>::Type>::TYPE RetType;
		typedef R (*MsgSender)(id,SEL,A1,A2);
		ScriptState L(_L);
		id obj = (id)ScriptState::GetUnwind<void*>::get(L, lua_upvalueindex(1));
		SELF* thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));

		ScriptState::PushUnwind<R>::push(
			L,
			((MsgSender)objc_msgSend)(obj,thiz->mSelector,
				ScriptState::GetUnwind<A1>::get( L, 1 ),
				ScriptState::GetUnwind<A2>::get( L, 2 )
			),
			TYPE_FROM_NAME( WrapType )
			);
		return ScriptManager::getInstance()->return_(1);
	}

	virtual std::string	description(){
		return typeName<R>() + "(*selector)("
			+ typeName<A1>() + ','
			+ typeName<A2>() + ')';
	};
};

template<class A1, class A2>
class CHAOS_API ApplierOCRA2<void, A1, A2> : public ApplierOC{
public:
	typedef		ApplierRA2<void,A1,A2> SELF;
	ApplierOCRA2(SEL sel):ApplierOC(sel,SELF::lua_call){
	};

	static int lua_call(lua_State* _L){
		typedef void (*MsgSender)(id,SEL,A1,A2);
		ScriptState L(_L);
		id obj = (id)ScriptState::GetUnwind<void*>::get(L, lua_upvalueindex(1));
		SELF* thiz((SELF*)lua_touserdata(_L,lua_upvalueindex(2)));
		((MsgSender)objc_msgSend)(obj, thiz->mSelector,
			ScriptState::GetUnwind<A1>::get( L, 1 ),
			ScriptState::GetUnwind<A2>::get( L, 2 )
		);
		return ScriptManager::getInstance()->return_(0);
	}

	virtual std::string	description(){
		return typeName<void>() + "(*selector)("
			+ typeName<A1>() + ','
			+ typeName<A2>() + ')';
	};
};

#define SELECTOR2(sel, R, A1, A2)	new ApplierOCRA2<R,A1,A2>(sel)
