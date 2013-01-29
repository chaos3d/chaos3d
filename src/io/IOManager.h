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

#include "chaos_config.h"
#include "core/core.h"
#include "core/RTTI.h"
#include <string>
#include <map>

namespace pcrecpp{
	class RE;
};

_CHAOS_BEGIN

class DataStream;
class DataSource;
class StreamLocator;

/*
Very like file manager, but not limited on file system, but expend to
network. Every data stream is denoted as a unique string, like URL,
and io manager will parse it and return a data stream if valid.
*/
DECLARE_CLASS(IOManager, NilParent);
class CHAOS_API IOManager : public Singleton<IOManager>{

	DECLARE_NOTCOPYABLE(IOManager);
	DECLARE_NOTASSIGNABLE(IOManager);
protected:
	//default locations, such as app/doc
	typedef std::multimap<std::string, StreamLocator*>	TFileMap;
	std::string		mDocPath, mApplicationPath, mExternalPath;
	TFileMap		mFileLoc;
	pcrecpp::RE*	mUrlRE;

	void			registerDefaults();
public:
	enum{
		MAX_URL_LEN = 256
	};

	IOManager();
	~IOManager();

	/*
	to get a data stream by url

	e.g.
	app://texture.png
	doc://savefile.xml
	mem://memdata
	http://url.com

	*/
	DataStream*		streamByURL( char const* url );
	DataStream*		streamByURL( std::string const& url ){
		return streamByURL( url.c_str() );
	}
	
	DataSource*		open( char const* url, char const* mode );

	std::string		nameByURL( char const* url );

	void	addPackage( char const* base, char const* url );

	std::string	docPath() const { return mDocPath; };
	std::string applicationPath() const { return mApplicationPath; };
	// only for sdcard
	std::string externalPath() const { return mExternalPath; };
};

_CHAOS_END

#endif

