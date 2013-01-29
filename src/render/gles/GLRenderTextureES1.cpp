#include "2d/gles/GLRenderTextureES1.h"

//using namespace chaos;

GLRenderTextureES1::GLRenderTextureES1(GLTexture* tex) 
	: RenderTexture(tex), mCurrentBuffer(0), mCurrentContext(0), mCurrentSurface(0)
{
	mCurrentDisplay = eglGetCurrentDisplay();

	setTexture(tex);
}

GLRenderTextureES1::~GLRenderTextureES1(){
}

void GLRenderTextureES1::bind(){
	mCurrentContext = eglGetCurrentContext();
	mCurrentSurface = eglGetCurrentSurface(EGL_DRAW);

	// Switch the render target to the pBuffer
	if(!eglMakeCurrent(mCurrentDisplay, mPBufferSurface[mCurrentBuffer],
		mPBufferSurface[mCurrentBuffer], mCurrentContext))
	{
		LOG("Unable to make the pbuffer context current.");
	}

}

void GLRenderTextureES1::flush(){
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,((GLTexture*)mTarget)->texId() );

	// We've now done rendering to the PBuffer Surface
	if(!eglBindTexImage(mCurrentDisplay, mPBufferSurface[mCurrentBuffer], EGL_BACK_BUFFER)){
		LOG("Failed to bind m_PBufferSurface.");
	}

	mCurrentBuffer = !mCurrentBuffer;
	if(!eglReleaseTexImage(mCurrentDisplay, mPBufferSurface[mCurrentBuffer], EGL_BACK_BUFFER)){
		LOG(" Failed to release m_PBufferSurface.");
	}

	// We now switch back to the backbuffer for rendering.
	if(!eglMakeCurrent(mCurrentDisplay, mCurrentSurface, mCurrentSurface, mCurrentContext)){
		LOG("ERROR: Unable to make the main context current.\n");
	}
}

void GLRenderTextureES1::destroy(){
	eglDestroySurface(mCurrentDisplay, mPBufferSurface[0]);
	eglDestroySurface(mCurrentDisplay, mPBufferSurface[1]);
}

void GLRenderTextureES1::setTexture(Texture* tex){
	if( tex != 0 ){
		tex->retain();
		
		destroy();

		// Set up a configuration and attribute list used for creating a PBuffer surface.
		EGLConfig eglConfig = selectConfig();

		Vector2f size (tex->getSize());
		EGLint list[]=
		{
			// First we specify the width of the surface...
			EGL_WIDTH, (int)size[0],
			// ...then the height of the surface...
			EGL_HEIGHT, (int)size[1],
			/* ... then we specifiy the target for the texture
				that will be created when the pbuffer is created...*/
			EGL_TEXTURE_TARGET, EGL_TEXTURE_2D,
			/*..then the format of the texture that will be created
				when the pBuffer is bound to a texture...*/
			EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGB,
			// The final thing is EGL_NONE which signifies the end.
			EGL_NONE
		};

		mPBufferSurface[0] = eglCreatePbufferSurface(mCurrentDisplay, eglConfig, list);
		mPBufferSurface[1] = eglCreatePbufferSurface(mCurrentDisplay, eglConfig, list);
		mCurrentBuffer = 0;

		if(mPBufferSurface[0] == EGL_NO_SURFACE || mPBufferSurface[1] == EGL_NO_SURFACE)
			LOG("Unable to create PBuffer.");
	}

	if( mTarget != 0 )
		mTarget->release();

	mTarget = tex;
}



EGLConfig GLRenderTextureES1::selectConfig()
{
	EGLConfig EglConfig = 0;

	EGLint i32ConfigID;
	int i32BufferSize;

	// Get the colour buffer size of the current surface so we can create a PBuffer surface
	// that matches.
	EGLDisplay eglDisplay = eglGetCurrentDisplay();
	eglQueryContext(eglDisplay, eglGetCurrentContext(), EGL_CONFIG_ID, &i32ConfigID);
	eglGetConfigAttrib(eglDisplay, (EGLConfig) i32ConfigID, EGL_BUFFER_SIZE,&i32BufferSize);

    EGLint i32ConfigNo;

	// Setup the configuration list for our surface.
    EGLint conflist[] =
	{
		EGL_CONFIG_CAVEAT, EGL_NONE,
		/*
			Tell it the minimum size we want for our colour buffer and the depth size so
			eglChooseConfig will choose the config that is the closest match.
		*/
		EGL_BUFFER_SIZE, i32BufferSize,
		EGL_DEPTH_SIZE, 16,
		// The PBuffer bit is the important part as it shows we want a PBuffer
		EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
		EGL_BIND_TO_TEXTURE_RGB, EGL_TRUE,
		EGL_NONE
	};

	// Find and return the config 
    if(!eglChooseConfig(mCurrentDisplay, conflist, &EglConfig, 1, &i32ConfigNo) || i32ConfigNo != 1){
		LOG("Unable to find a suitable config.\n");
		return 0;
    }

    return EglConfig;
}
