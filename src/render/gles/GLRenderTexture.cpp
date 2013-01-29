#include "2d/gles/GLRenderTexture.h"
#include "2d/gles/GLTexture.h"

//using namespace chaos;

GLRenderTexture::GLRenderTexture(GLTexture* tex) : RenderTexture(tex), mFrameID(0){
	setTexture(tex);
}

void GLRenderTexture::setTexture(Texture* tex){
	if( tex != 0 ){
		tex->retain();
		
		destroy();

		glGenFramebuffersOES(1, &mFrameID);			
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, mFrameID);
		glFramebufferTexture2DOES( GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, ((GLTexture*)tex)->texId(), 0);
	}

	if( mTarget != 0 )
		mTarget->release();

	mTarget = tex;
}

void GLRenderTexture::destroy(){
	if( glIsFramebufferOES( mFrameID ) )
		glDeleteBuffers( 1, &mFrameID);
}

GLRenderTexture::~GLRenderTexture(){
	destroy();
}

void GLRenderTexture::bind(){
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, mFrameID);
}

void GLRenderTexture::flush(){
}
