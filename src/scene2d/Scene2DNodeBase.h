template<BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(LUABIND_MAX_ARITY, class A, detail::null_type)>
class Scene2DNodeBase : public ReferencedCount 
{
public:
	typedef BOOST_PP_CAT(
			boost::mpl::vector, BOOST_PP_INC(BOOST_PP_INC(LUABIND_MAX_ARITY)))<
		void, argument const&, BOOST_PP_ENUM_PARAMS(LUABIND_MAX_ARITY, A)
		> components0;

	typedef typename boost::mpl::remove<
		components0, detail::null_type>::type components;

	template<class C>
	C* create(C const& _c) {
		C*& c = static_cast<wrap<C> >(_components).value;
		SAFE_DELETE(c);
		c = new C(_c);
		return c;
	};

	template<class C>
	C* get() {
		return static_cast<wrap<C> >(_components).value;
	}

protected:
	typedef typename boost::mpl::remove_if<
		components, scene2d::has_clear_flag<_> >::type components_has_clear_flag;
	typedef typename boost::mpl::remove_if<
		components, scene2d::needs_update<_> >::type components_needs_update;

	template<class C> struct wrap { C* value; };
	boost::mpl::inherit_linearly<
		components, boost::mpl::inherit<wrap<_2>, _1> 
		>::type	_componenets;
};
