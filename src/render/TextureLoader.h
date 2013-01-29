/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_TEXTURELOADER_H
#define	_CHAOS_TEXTURELOADER_H

#include "res/ResourceLoader.h"

_CHAOS_BEGIN

class TexturePNGLoader : public ResourceLoader{

protected:
public:
	virtual int		type(){
		return '\0png';
	};

	virtual Resource*	load( DataStream* ds, char const* name );
	virtual void		unload( Resource* res );
};

class TexturePVRLoader : public ResourceLoader{
};

_CHAOS_END

#endif

