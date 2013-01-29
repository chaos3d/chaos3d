/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "res/AudioLoader.h"
#include "core/core.h"
using namespace chaos;

/*
 OpenAL may not be supported for all platforms, i.e. android,
 this is a placeholder for non-openal platform
*/
AudioLoader::AudioLoader(){
}

Resource* AudioLoader::load( DataStream* ds, char const* ){
	LOG("Audio loader is not supported for this build.");
	return 0;
}
