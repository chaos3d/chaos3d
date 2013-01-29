/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "res/ChunkLoader.h"

//using namespace chaos;

void ChunkLoader::unload( Resource* res ){
	ResourceChunk *rc = (ResourceChunk*)res;
	delete rc;
}

Resource* ChunkLoader::load( DataStream* ds, char const* name ){
	return new ResourceChunk( name );
}
	
