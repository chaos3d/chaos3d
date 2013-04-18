/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_IOMANAGER_H
#define	_CHAOS_IOMANAGER_H

#include "common/common.h"
#include "common/Singleton.h"

#include <string>
#include <map>
#include <vector>
#include "boost/smart_ptr.hpp"

_CHAOS_BEGIN

class DataStream;
class DataSource;
class StreamLocator;

/*
Very like file manager, but not limited on file system, but expend to
network. Every data stream is denoted as a unique string, like URL,
and io manager will parse it and return a data stream if valid.
*/
class CHAOS_API IOManager : public Singleton<IOManager>{
public:
	enum{
		MAX_URL_LEN = 256
	};

	IOManager();
	~IOManager();

	// to create a data stream from a path
	DataStream* createStreamByPath(char const* path);
	DataStream*	createStreamByPath(std::string const& path){
		return createStreamByPath(path.c_str());
	}
	
	DataSource*		open( char const* url, char const* mode );

	std::string		nameByURL( char const* url );

	std::string	docPath() const { return mDocPath; };
	std::string applicationPath() const { return mApplicationPath; };
	// only for sdcard
	std::string externalPath() const { return mExternalPath; };

    // adding locators
    bool addLocator(StreamLocator*);
    bool addPkgLocator(char const*, int priority = 0);
    bool addDirLocator(char const*, int priority = 0);
    bool addZipLocator(char const*, int priority = 0);
    
private:
	//default locations, such as app/doc
	//typedef std::multimap<std::string, StreamLocator*>	TFileMap;
	//TFileMap		mFileLoc;
	std::string		mDocPath, mApplicationPath, mExternalPath;
    
    typedef std::vector<boost::shared_ptr<StreamLocator> > TLocator;
    TLocator _locators;
    
	//void			registerDefaults();
};

_CHAOS_END

#endif

