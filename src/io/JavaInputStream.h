/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#ifndef	_CHAOS_JAVAINPUTSTREAM_H
#define	_CHAOS_JAVAINPUTSTREAM_H

#include "chaos_config.h"
#include "platform/jni/jni.h"
#include "io/DataStream.h"
#include <string>
#include <vector>

_CHAOS_BEGIN

class CHAOS_PRIVATE JavaInputStream : public DataStream{
public:
	typedef std::vector<std::string>	FileList;

private:
	static jclass		JByteBuffer;
	static jmethodID	midRead, midPosition, midSeek, midSize, midStartOff;
	static jmethodID	midCreateIS, midGetChannel, midWrap;
	static jmethodID	midOpenFD, midList;

	bool create(char const* filename);
	static void initMID(JNIEnv *env);

protected:
	size_t	mSize, mOffset, mStartOff;
	jobject	mInput;
	std::string const	mName;

public:
	~JavaInputStream();
	JavaInputStream(char const* filename);

	virtual bool	valid();
	virtual void	close();
	virtual bool	end();
	virtual size_t	read( void* buf, size_t bufSize );
	virtual bool	seek( long offset, int pos );
	virtual long	tell();
	virtual char const* where() { return mName.c_str(); }

	static JavaInputStream* open(char const* filename);
	static FileList list(char const* dir);
};

_CHAOS_END

#endif
