/**
 * Load/Unload resources from the I/O
 * Reload resources
 */
class Resource {
public:
	typedef boost::shared_ptr<ResourceSource> SourcePtr;

	uint64_t UDID();

	// load the resource from the IO
	virtual bool load(SourcePtr const&) = 0;
	// unload the resource
	virtual bool unload() = 0;
	// swap the resource out of the memory
	virtual bool offline() = 0;
	// reload the resouce
	virtual bool reload() = 0;
	virtual bool is_loaded() = 0;

protected:
	uint64_t _udid;
	SourcePtr _source;
};
