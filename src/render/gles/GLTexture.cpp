#include "core/core.h"
#include "2d/gles/GLTexture.h"

//using namespace chaos;

const GLenum GLTexture::wrapMap[2] = {
	GL_CLAMP_TO_EDGE,
	GL_REPEAT,
};

const GLenum GLTexture::filterMap[5] = {
	GL_LINEAR,
	GL_NEAREST_MIPMAP_NEAREST,
	GL_NEAREST,
	GL_NEAREST_MIPMAP_LINEAR,
	GL_LINEAR_MIPMAP_LINEAR,
};

GLTexture::~GLTexture(){
	glDeleteTextures(1, &mTexId);
}

GLTexture::GLTexture(int type, int minFilter, int magFilter, int wrapS, int wrapT, bool genMipmap)
	: Texture( type, minFilter, magFilter, wrapS, wrapT, genMipmap )
{
	glGenTextures(1, &mTexId);
	setParameters();
}

void GLTexture::loadSubData(void* data,int x, int y, int width, int height, int format){
	glBindTexture(GL_TEXTURE_2D, mTexId);

	switch(format)
	{
	case Format2D_RGBA8888:
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		break;
	case Format2D_RGB565:
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data);
		break;
	case Format2D_A8:
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_ALPHA, GL_UNSIGNED_BYTE, data);
		break;
	default:
		LOG_STAT( ERROR, "Not a supported texture format." );
		break;			
	}

	if( glGetError() != GL_NO_ERROR ){
		LOG_STAT( ERROR, "Unable to load sub data into existing texture. (%d)", mTexId);
	}
}

void GLTexture::loadData(void* data, int width, int height, int level, int format, size_t dataSize){
	
	GLint saveName;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &saveName);
	glEnable( GL_TEXTURE_2D );
	glBindTexture(GL_TEXTURE_2D, mTexId);
	glActiveTexture(GL_TEXTURE0);
	
	switch(format)
	{
	case Format2D_RGBA8888:
		glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		break;

	case Format2D_RGB565:
		glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data);
		break;

	case Format2D_A8:
		glTexImage2D(GL_TEXTURE_2D, level, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
		break;

#if defined(__APPLE__)
	case Format2D_RGBA_PVRTC_4BPPV1:
		glCompressedTexImage2D( GL_TEXTURE_2D, level, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, width, height, 0, dataSize, data);
		break;

	case Format2D_RGB_PVRTC_4BPPV1:
		glCompressedTexImage2D( GL_TEXTURE_2D, level, GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, width, height, 0, dataSize, data);
		break;

	case Format2D_RGBA_PVRTC_2BPPV1:
		glCompressedTexImage2D( GL_TEXTURE_2D, level, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, width, height, 0, dataSize, data);
		break;

	case Format2D_RGB_PVRTC_2BPPV1:
		glCompressedTexImage2D( GL_TEXTURE_2D, level, GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG, width, height, 0, dataSize, data);
		break;
#endif

	default:
		LOG_STAT( ERROR, "Not a supported texture format." );
		break;			
	}
	
	//int i = glGetError();
	if( glGetError() != GL_NO_ERROR ){
		LOG_STAT( ERROR, "Unable to load a texture.");
	}

	glBindTexture(GL_TEXTURE_2D, saveName);
	
	mSize = Vector2f((float)width, (float)height);
}

void GLTexture::setParameters(){
	GLint saveName;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &saveName);
	glBindTexture(GL_TEXTURE_2D, mTexId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMap[mMinFilter]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMap[mMagFilter]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMap[mWrapS]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMap[mWrapT]);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, (mGenMipmap?GL_TRUE:GL_FALSE));
	glBindTexture(GL_TEXTURE_2D, saveName);
}

void GLTexture::bind(){
	glBindTexture(GL_TEXTURE_2D, mTexId);
#ifdef	DEBUG
	if( glGetError() != GL_NO_ERROR ){
		LOG_STAT( ERROR, "Unable to bind the texture.");
	}
#endif
}

