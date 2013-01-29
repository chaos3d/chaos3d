/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "io/StreamLocator.h"
#include "io/FileStream.h"
#include "io/PkgChunkStream.h"
#include "io/IOManager.h"
#include "core/utils.h"
#include "pcre/pcrecpp.h"

using namespace std;
//using namespace chaos;
using namespace io;

#ifdef	WIN32
static pcrecpp::RE s_fileRE("/");	// to replace '/' to '\'
#endif

bool DirLocator::exist(const char *cpath){
	string path(mBase + cpath);
#ifdef	WIN32
	// translate '/' to '\'
	s_fileRE.GlobalReplace( "\\\\", &path );
#endif

	return FileStream::exist( path.c_str() );
}

DataStream* DirLocator::stream(const char *cpath){
	string path(mBase + cpath);
#ifdef	WIN32
	// translate '/' to '\'
	s_fileRE.GlobalReplace( "\\\\", &path );
#endif

	DataStream* ds = new FileStream( path.c_str() );
	ds->autorelease();
	return ds;
}

std::string	DirLocator::url( const char* path){
	return mBase + path;
}

///-----------------------------------------------------
///-----------------------------------------------------
PkgFileLocator::~PkgFileLocator(){
	delete mPkgFile;
}

PkgFileLocator::PkgFileLocator( const char* pkg ) : mFilename(pkg), mPkgFile(0){
	if( !FileStream::exist( pkg ) )
		return;

	char chunk[4];
	FileStream fspatch( pkg );
	FileStream* fs = &fspatch;

	fs->read( chunk, 4 );
	if( memcmp( chunk, ".pkg", 4 ) != 0 )
		return;

	fs->read( chunk, 4 );
	if( memcmp( chunk, "file", 4 ) != 0 )
		return;

	long size(0);
	fs->read( &size, 4 );

	char*	buffer = new char[size];
	char*	orig = buffer;
	long	offset = fs->tell();
	size = fs->read( buffer, size );

	while( size > 0 ){
		char*	filename = buffer;
		size_t	fl = strlen(filename);
		long	file_offset = offset;	//todo: bug, big/little endian

		mHash.insert( pair<unsigned long,long>( hash_djb2(filename), file_offset) );
		size -= fl + 9;
		buffer += fl + 9;
		offset += fl + 9;
	}

	mPkgFile = new FileStream( pkg );
	delete [] orig;
}

bool PkgFileLocator::exist(char const *path){
	typedef TFileHash::iterator iterator;
	typedef pair<iterator,iterator> result;

	result ret = mHash.equal_range( hash_djb2( path ) );
	if( ret.first == ret.second )
		return false;

	char buffer[IOManager::MAX_URL_LEN];
	size_t pl = strlen( path );

	for( iterator it = ret.first; it != ret.second; ++it ){
		mPkgFile->seek( it->second, DataStream::Seek_Set );
		mPkgFile->read( buffer, pl+1 );

		if( memcmp( path, buffer, pl ) == 0 && buffer[pl]  == '\0'){
			return true;
		}
	}

	return false;
}

DataStream* PkgFileLocator::stream(const char *path){
	typedef TFileHash::iterator iterator;
	typedef pair<iterator,iterator> result;

	result ret = mHash.equal_range( hash_djb2( path ) );
	if( ret.first == ret.second )
		return 0;

	DataStream* ds(0);
	char buffer[IOManager::MAX_URL_LEN];
	size_t pl = strlen( path );

	for( iterator it = ret.first; it != ret.second; ++it ){
		mPkgFile->seek( it->second, DataStream::Seek_Set );
		mPkgFile->read( buffer, pl+1 );

		if( memcmp( path, buffer, pl ) == 0 && buffer[pl]  == '\0'){
			long offset(0);
			size_t size(0);
			mPkgFile->read( &offset, 4 );
			mPkgFile->read( &size, 4 );
			DataStream *fs = new FileStream(mFilename.c_str());
			ds = new PkgChunkStream(fs, offset + 4, size, (mFilename + '&' + path).c_str());
			fs->release();
			break;
		}
	}

	if( ds != 0 )
		ds->autorelease();
	return ds;
}

std::string	PkgFileLocator::url( const char* path){
	return mFilename;
}
