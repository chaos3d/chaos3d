#ifndef _RESOURCE_H
#define _RESOURCE_H

/**
 * load expensive data from IO i.e. file/network to memory
 * keeping the lightweight data
 */
class resource {
public:
	virtual ~resource() {};

	// load the resource from the IO
	virtual bool load() = 0;

	// unload the resource
	virtual bool unload() = 0;

	// swap the resource out of the memory
	virtual bool offline() = 0;

	// reload the resouce
	virtual bool is_loaded() = 0;

	virtual bool is_loading() = 0;
};

template<class R>
class resource_base : public resource {

};

#endif
