template<class Resource>
class ResourceID {
public:
	Resource* get();

protected:
	Resource* _resource;
};
