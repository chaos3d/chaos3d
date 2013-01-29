/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "io/IOManager.h"
#include "script/bind.h"
#include "pcre/pcrecpp.h"

#include "io/StreamLocator.h"
#include "io/APKAssetsLocator.h"
#include "io/FileStream.h"
#include "io/FileSource.h"
#include "io/MemoryDataStream.h"
#include "io/GlobalMDSManager.h"

#include "io/DataStream.h"
#include "io/DataSource.h"
#include "io/DataStreamFactory.h"

#ifdef	WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif

//using namespace chaos;
using namespace std;

IMPLEMENT_CLASS(IOManager, LIB_DOMAIN)
IMPLEMENT_FUNC(addPackage, &IOManager::addPackage)
IMPLEMENT_FUNC(streamByURL, (DataStream* (IOManager::*)(char const*))&IOManager::streamByURL)
IMPLEMENT_FUNC(open, &IOManager::open)
IMPLEMENT_FUNC(path, &IOManager::nameByURL)
IMPLEMENT_PROP(docPath, 0, &IOManager::docPath)
IMPLEMENT_PROP(applicationPath, 0, &IOManager::applicationPath)
IMPLEMENT_END;

IOManager::IOManager() : mUrlRE( new pcrecpp::RE("(\\w+)://(.+)$"))
{
	registerDefaults();
}

IOManager::~IOManager(){
	delete mUrlRE;
}

#ifdef	WIN32
void IOManager::registerDefaults(){
	TCHAR szDirectory[MAX_PATH];
	char* base_loc[2] = {"app","doc"};
	string base[2];

	if(!::GetCurrentDirectory(sizeof(szDirectory) - 1, szDirectory)){
		LOG( "Unable to get the current dictory." );
	}else{
		base[0] = string(szDirectory) + '\\';
		base[1] = (string(szDirectory) + "\\doc\\");

	}

	// automatically add the .pkg file into locators
	WIN32_FIND_DATA found;
	HANDLE sHandle(0);

	for( int i=0;i<2; ++i ){
		mFileLoc.insert( pair<string,StreamLocator*>( base_loc[i], 
			new io::DirLocator( base[i].c_str() ) ));
	
		sHandle = FindFirstFile( (base[i]+"*.pkg").c_str(), &found );
		while( sHandle != 0){
			if( memcmp( found.cFileName + strlen( found.cFileName ) - 4,".pkg", 4) == 0){
				mFileLoc.insert( pair<string,StreamLocator*>( base_loc[i], 
					new io::PkgFileLocator( found.cFileName ) ));
			}

			if( FindNextFile( sHandle, &found) == FALSE )
				break;
		}
	}

	if( sHandle != 0 )
		FindClose( sHandle );
	
}
//#elif !defined(__APPLE__)
#elif defined(ANDROID)
// temp code for linux
void IOManager::registerDefaults(){
	// only add assets folder now
	// shouldn't further pack up the resources
	mFileLoc.insert( pair<string,StreamLocator*>( string("app"), 
				new io::APKAssetsLocator("")));
}
#elif defined(OS_NIX)
void IOManager::registerDefaults(){
	char cDirectory[FILENAME_MAX];
	char const* base_loc[2] = {"app","doc"};
	string base[2];

	if(!::getcwd(cDirectory, sizeof(cDirectory))){
		LOG( "Unable to get the current dictory." );
	}else{
		base[0] = string(cDirectory) + '/';
		base[1] = (string(cDirectory) + "/doc/");
	}

	mApplicationPath = base[0];
	mDocPath = base[1];
	
	// automatically add the .pkg file into locators
	for( int i=0;i<2; ++i ){
		mFileLoc.insert( pair<string,StreamLocator*>( base_loc[i], 
			new io::DirLocator( base[i].c_str() ) ));
	
		struct dirent *ep;
		DIR* dir = opendir(base[i].c_str());
		if( dir != 0 ) while(ep = readdir(dir)){
			if( ep->d_type == DT_REG && memcmp("pkg",ep->d_name+strlen(ep->d_name)-3,3) == 0 ){
				mFileLoc.insert( pair<string,StreamLocator*>( base_loc[i], 
					new io::PkgFileLocator(ep->d_name) ));
			}
		}
	}
}

#elif !defined(CHAOS_OSX)	// defined in mm file
void IOManager::registerDefaults(){
}
#endif

void IOManager::addPackage( char const* base, char const* url ){
	mFileLoc.insert( pair<string,StreamLocator*>( base, 
		new io::PkgFileLocator( url ) ));
}

std::string IOManager::nameByURL( char const* url ){
	string prefix, post;

	if(!mUrlRE->FullMatch( url, &prefix, &post )){
		LOG("URL not correct: %s", url);
		return std::string("");
	}

	TFileMap::iterator it = mFileLoc.find( prefix );
	if( it != mFileLoc.end() ){
		return it->second->url( post.c_str() );
	}else
		return (prefix += post);
}

DataStream* IOManager::streamByURL( char const* url ){
	string prefix, post;

	if(!mUrlRE->FullMatch( url, &prefix, &post )){
		LOG("URL not correct: %s", url);
		return 0;
	}

	DataStream* ds = 0;
	typedef TFileMap::iterator iterator;
	typedef pair<iterator, iterator> result;
	result ret = mFileLoc.equal_range( prefix );
	if( ret.first != ret.second ){		// find a locator

		for(iterator it = ret.first; it != ret.second; ++it ){
			if( it->second->exist( post.c_str() ) ){
				return it->second->stream( post.c_str() );
			}
		}
		LOG("Unable to locate from the url (%s).", url);
		return 0;
	}else if( prefix == "mem" ){
#ifdef	DEBUG
		if( GlobalMDSManager::getInstance() == 0 ){
			LOG("Global memory manager is not correctly created: %s", url );
			return 0;
		}
#endif
		ds = GlobalMDSManager::getInstance()->streamByName( post.c_str() );
	}else if( prefix == "http" ){	// a http request
	}else if( prefix == "ftp" ){	// a ftp request
	}else{
		LOG("Not a supported location: %s", url );
	}

	return ds;
}

DataSource* IOManager::open( char const* url, char const* mode ){
	FileSource* file = new FileSource( nameByURL(url).c_str(), mode );
	file->autorelease();
	return file;
}

