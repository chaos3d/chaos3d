/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_APKASSETSLOCATOR_H
#define	_CHAOS_APKASSETSLOCATOR_H

#include "chaos_config.h"
#include "io/StreamLocator.h"
#include <set>
#include <string>

_CHAOS_BEGIN

namespace io{

	class CHAOS_PRIVATE APKAssetsLocator : public StreamLocator{
	protected:
		typedef std::set<std::string>	TFileHash;

		TFileHash		mHash;
		std::string		mFilename;
		std::string		mBase;

	public:
		APKAssetsLocator(char const* dir);
		~APKAssetsLocator();

		virtual bool	exist(const char* path );
		virtual DataStream*	stream(const char* path);
		virtual std::string	url(const char* path);
	};

};

_CHAOS_END

#endif
