#include "2d/gles/GLRenderWindow-android.h"
#include <2d/gles/glwrap.h>

//using namespace chaos;

GLRenderWindow::~GLRenderWindow(){
};

GLRenderWindow::GLRenderWindow(){
};

void GLRenderWindow::bind(){
	// unbind buffers
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, 0);
}

void GLRenderWindow::flush(){
}
