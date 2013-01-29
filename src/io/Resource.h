/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef _CHAOS_RESOURCE_H
#define _CHAOS_RESOURCE_H

#include "chaos_config.h"
#include "core/RTTI.h"
#include "core/core.h"
#include "script/ScriptState.h"

_CHAOS_BEGIN

class ResourceManager;
class ResourceLoader;

/** Resource
*/
DECLARE_CLASS(Resource, NilParent);
class CHAOS_API Resource : public ReferencedCount{
	DYNAMIC_CLASS(Resource);

	friend class ResourceLoader;
	friend class ResourceManager;
public:
	typedef SharedPtr<Resource>	Ptr;

protected:
	///** Only create the instance but let the data alone.
	//*/
	//virtual int		create() = 0;

	///*
	//load and unload will be called by ResourceGroup.
	//One resource cannot be literally loaded or unloaded.
	//*/
	///** Load the data into memory
	//*/
	//virtual	int		load() = 0;

	///** Unload the data
	//*/
	//virtual void	unload() = 0;

	///*
	//the lifetime of resources are managed by resource allocater,
	//so it cannot be literally created or destroyed.
	//*/
	virtual ~Resource(){
	};
public:

	/*
	to allocate memory from global resource allocator
	*/
	/*
	may overload those operators to apply stack allocations or pool allocations,
	it will then connect to resource mananger internally for general resources,
	and also for some particlar resources, it can apply other policies by over-
	loading new operators.

	NB: some factories may use placement new operator. then those placement new
	operator should be overloaded as well in order in some cases to track what 
	and how	many resources are allocated, or more even, for debugging, not allow-
	ing placement new by returning another address managed by debug system.
	*/
	//void*	operator new(size_t size);

	/*
	release nothing. allocator will handle this.
	*/
	//void	operator delete(void*){};
};

// we don't want Resource referenced by scripts because it's
// managed by the resouce manager
TYPE_RET_REF( Resource );
TYPE_RET_DYN( Resource );

_CHAOS_END

#endif
