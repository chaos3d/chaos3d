/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "core/core.h"
#include "2d/gles/GLVertexBuffer.h"

//using namespace chaos;

GLVertexBuffer::GLVertexBuffer( size_t sizeInBytes, void* data, int usage ) 
	: HardwareBuffer(sizeInBytes)
{
	glGenBuffers( 1, &mBufferId );
 
	glBindBuffer( GL_ARRAY_BUFFER, mBufferId );
	glBufferData( GL_ARRAY_BUFFER, sizeInBytes, data, usage );

	if( glGetError() != GL_NO_ERROR )
		LOG_STAT( ERROR, "Unable to create a vertex buffer." );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

GLVertexBuffer::~GLVertexBuffer(){
	glDeleteBuffers( 1, &mBufferId );
	mBufferId = 0;
}

void* GLVertexBuffer::lock(){
	mIsLocked = true;
	glBindBuffer( GL_ARRAY_BUFFER, mBufferId );
	return glMapBufferOES( GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES);
}

void GLVertexBuffer::unLock(){
	mIsLocked = false;
	glBindBuffer( GL_ARRAY_BUFFER, mBufferId );
	glUnmapBufferOES( GL_ARRAY_BUFFER);
}

void* GLVertexBuffer::getOffset( int offset ){
	return (char*)0 + offset;
}

void GLVertexBuffer::bind(){
	mIsBind = true;
	glBindBuffer( GL_ARRAY_BUFFER, mBufferId );
}

void GLVertexBuffer::unBind(){
	mIsBind = false;
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

GLVertexClientBuffer::GLVertexClientBuffer( size_t sizeInBytes, void* data, int usage ) 
	: HardwareBuffer(sizeInBytes)
{
	mBuffer = new char[sizeInBytes];

	if( mBuffer == 0 ){
		LOG_STAT( ERROR, "Unable to create a vertex buffer. out of memory." );
		return ;
	}

	if( data != 0 )
		memcpy( mBuffer, data, sizeInBytes );

}

GLVertexClientBuffer::~GLVertexClientBuffer(){
	delete [] mBuffer;
}

void* GLVertexClientBuffer::lock(){
	return mBuffer;
}

void GLVertexClientBuffer::unLock(){
}

void* GLVertexClientBuffer::getOffset( int offset ){
	return (char*)mBuffer + offset;
}

void GLVertexClientBuffer::bind(){
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	mIsBind = true;
}

void GLVertexClientBuffer::unBind(){
	mIsBind = false;
}
