template<class Source, class Resource>
struct ResourceLoader{
	static Resource* load(Source const&);
};

class ResourceManager {
public:
	template<class S, class R>
	R* load(Resource::SourcePtr const& src){
		return ResourceLoader<S,R>::load(src);
	}
};
