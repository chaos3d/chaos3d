/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_SCRIPTREFLOADER_H
#define	_CHAOS_SCRIPTREFLOADER_H

#include "ResourceLoader.h"
#include <map>

_CHAOS_BEGIN

/*
a resource that refers to a object in script

it will not be unloaded, because it cannot be created again if lost ref.
*/
class CHAOS_PRIVATE ScriptRefLoader : public ResourceLoader{
public:
	virtual int		type(){
		return 'scrf';
	};

	virtual Resource*	load( DataStream* ds, char const* name );
	virtual void		unload( Resource* res );

};

/*
a resource that creates by a function as a return value.

the function code will be dumped to memory or file piece. 
when loading, the return object will be refered to maintain
its lifetime, and unrefered after unloading, and then g-ced
by the script.

*/
class CHAOS_PRIVATE ScriptResourceLoader : public ResourceLoader{
protected:
	typedef std::map<Resource*, int>	TRefTable;

	/*
	once running the function, the return value should be retained
	in order to expend its lifetime and released after unloading,
	so the there will be a table storing which key the resource is
	using.
	*/
	TRefTable		mRefTable;
public:
	virtual int		type(){
		return 'scrs';
	};

	virtual Resource*	load( DataStream* ds, char const* name );
	virtual void		unload( Resource* res );

};

/*
a script resource
*/
class CHAOS_PRIVATE ScriptLoader : public ResourceLoader{
public:
	virtual int		type(){
		return 'scpt';
	};

	virtual Resource*	load( DataStream* ds, char const* name );
	virtual void		unload( Resource* res );

};

_CHAOS_END

#endif