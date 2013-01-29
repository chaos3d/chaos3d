/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "res/AudioLoader.h"
#include "audio/audio.h"
#include "io/DataStream.h"

//using namespace chaos;

AudioLoader::AudioLoader(){
}

Resource* AudioLoader::load( DataStream* ds, char const* ){
	AudioDecoder* decoder = AudioDecoder::decoderByStream(ds);

	if( decoder == 0 )
		return 0;

	AudioTrack* track = new AudioTrack();
	if( !decoder->getInfo( ds, track->getInfo() ) ){
		LOG("Failed to retreive information of the audio data.");
		delete track;
		return 0;
	}

	char* buffer = new char[track->getInfo().size];
	ds->seek( track->getInfo().offset, DataStream::Seek_Set );
	
	if( !decoder->decode( ds, buffer, track->getInfo().size ) ){
		LOG("Failed when decoding the audio data.");
		delete [] buffer;
		delete track;
		return 0;
	}

	track->load( buffer, track->getInfo().size);

	// iphone use static buffer, which stores the data in the client area
#ifndef	__APPLE__
	delete [] buffer;
#endif

	track->autorelease();
	return track;
}
