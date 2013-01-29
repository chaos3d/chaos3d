/* cHaos3D
 *
 * Copyright (C) 2009-2010 reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license in the license sub-directory.
 *
 */

#include "core/core.h"
#include "2d/gles/GLVertexIndexBuffer.h"

//using namespace chaos;

GLVertexIndexBuffer::GLVertexIndexBuffer( size_t sizeInBytes, void* data, int usage ) 
	: HardwareBuffer(sizeInBytes)
{
	glGenBuffers( 1, &mBufferId );
 
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mBufferId );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeInBytes, data, usage );

	if( glGetError() != GL_NO_ERROR )
		LOG_STAT( ERROR, "Unable to create a vertex index buffer." );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

GLVertexIndexBuffer::~GLVertexIndexBuffer(){
	glDeleteBuffers( 1, &mBufferId );
	mBufferId = 0;
}

void* GLVertexIndexBuffer::lock(){
	mIsLocked = true;
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mBufferId );
	return glMapBufferOES( GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY_OES);
}

void GLVertexIndexBuffer::unLock(){
	mIsLocked = false;
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mBufferId );
	glUnmapBufferOES( GL_ELEMENT_ARRAY_BUFFER);
}

void* GLVertexIndexBuffer::getOffset( int offset ){
	return (char*)0 + offset;
}

void GLVertexIndexBuffer::bind(){
	mIsBind = true;
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mBufferId );
}

void GLVertexIndexBuffer::unBind(){
	mIsBind = false;
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

GLVertexIndexClientBuffer::GLVertexIndexClientBuffer( size_t sizeInBytes, void* data, int usage ) 
	: HardwareBuffer(sizeInBytes)
{
	mBuffer = new char[sizeInBytes];

	if( mBuffer == 0 ){
		LOG_STAT( ERROR, "Unable to create a vertex index buffer. out of memory." );
		return ;
	}

	if( data != 0 )
		memcpy( mBuffer, data, sizeInBytes );
}

GLVertexIndexClientBuffer::~GLVertexIndexClientBuffer(){
	delete [] mBuffer;
}

void* GLVertexIndexClientBuffer::lock(){
	return mBuffer;
}

void GLVertexIndexClientBuffer::unLock(){
	
}

void* GLVertexIndexClientBuffer::getOffset( int offset ){
	return (char*)mBuffer + offset;
}

void GLVertexIndexClientBuffer::bind(){
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void GLVertexIndexClientBuffer::unBind(){
}
