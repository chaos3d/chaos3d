/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_RESOURCEDESCRIPTOR_H
#define	_CHAOS_RESOURCEDESCRIPTOR_H

#include <list>

_CHAOS_BEGIN

class Resource;
class ResourceRef;

enum ResourceState{
	Loading,
	Loaded,
	Unloading,
	Unloaded
};
/*
A entry of the descriptor to describe the state of a certain resource,
including, loading state, location, object pointer, ref list, type etc.
*/
struct ResourceDescriptor{
	typedef std::list<ResourceRef*>		TRefList;
	int				type;
	char const*		name;
	char const*		url;
	Resource*		resource;
	ResourceState	state;

	TRefList		refs;		// tracking the reference, so that when resource unloads or moves, correct the pointer
								// TODO: rewrite to use free list.

	ResourceDescriptor( int t = 0, char const* n = 0, char const* u = 0 )
		: type(t), name(n), url(u), resource(0), state(Unloaded)
	{};
};

_CHAOS_END

#endif

