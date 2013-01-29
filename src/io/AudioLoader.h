/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_AUDIOLOADER_H
#define	_CHAOS_AUDIOLOADER_H

#include "ResourceLoader.h"

_CHAOS_BEGIN

//
// fully load the data and decode so that open al source can play it
//
// return AudioTrack
class CHAOS_PRIVATE AudioLoader : public ResourceLoader{
public:
	AudioLoader();

	virtual int		type(){
		return 'trac';		// audio track
	};

	virtual Resource*	load( DataStream* ds, char const* name );
};

_CHAOS_END

#endif

