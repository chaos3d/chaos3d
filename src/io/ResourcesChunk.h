// ----------------------------------------------
//
// ResourcesChunk.h
//
// Created by wuh (2/28)
// Copyright (c) 2009
//
// ----------------------------------------------


#ifndef	_CHAOS_RESOURCESCHUNK_H
#define	_CHAOS_RESOURCESCHUNK_H

namespace chaos{

	/*
	a bunch of resources, to manage their references and fixup table

	it should be a tool-side class. at runtime, only the data it generates
	matters, and that data will be processed by chunk loader.
	*/

	class ResourcesChunk{
	};

	/*
	memory layout

	resource name'\0'resource name'\0'....'\0''\0'
	a fixup table (will design afterwards)
	*/
};

#endif
