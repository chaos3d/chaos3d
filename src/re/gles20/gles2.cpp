#include "gles2.h"
#include <cstring>

#if defined(C3DGL_EXPORT_SDK_API)

PFNGLTEXSTORAGE1DEXTPROC glTexStorage1DEXT = nullptr;
PFNGLTEXSTORAGE2DEXTPROC glTexStorage2DEXT = nullptr;
PFNGLTEXSTORAGE3DEXTPROC glTexStorage3DEXT = nullptr;
PFNGLTEXTURESTORAGE1DEXTPROC glTexureStorage1DEXT = nullptr;
PFNGLTEXTURESTORAGE1DEXTPROC glTexureStorage2DEXT = nullptr;
PFNGLTEXTURESTORAGE1DEXTPROC glTexureStorage3DEXT = nullptr;

PFNGLMAPBUFFEROESPROC glMapBufferOES = nullptr;
PFNGLUNMAPBUFFEROESPROC glUnmapBufferOES = nullptr;
PFNGLGETBUFFERPOINTERVOESPROC glGetBufferPointervOES = nullptr;

PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES = nullptr;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES = nullptr;
PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES = nullptr;
PFNGLISVERTEXARRAYOESPROC glIsVertexArrayOES = nullptr;

void c3d_gl_load_extensions() {
	/* Retrieve GL extension string */
    const GLubyte* extensions = glGetString(GL_EXTENSIONS);
    
    // PowerVR SDK seems to be buggy for the texture storage on Mac OS
#if !TARGET_OS_MAC
    if (strstr((char*)extensions, "GL_EXT_texture_storage")) {
        glTexStorage1DEXT = (PFNGLTEXSTORAGE1DEXTPROC) eglGetProcAddress("glTexStorage1DEXT");
        glTexStorage2DEXT = (PFNGLTEXSTORAGE2DEXTPROC) eglGetProcAddress("glTexStorage2DEXT");
        glTexStorage3DEXT = (PFNGLTEXSTORAGE3DEXTPROC) eglGetProcAddress("glTexStorage3DEXT");
        glTexureStorage1DEXT = (PFNGLTEXTURESTORAGE1DEXTPROC) eglGetProcAddress("glTexureStorage1DEXT");
        glTexureStorage2DEXT = (PFNGLTEXTURESTORAGE1DEXTPROC) eglGetProcAddress("glTexureStorage2DEXT");
        glTexureStorage3DEXT = (PFNGLTEXTURESTORAGE1DEXTPROC) eglGetProcAddress("glTexureStorage3DEXT");
    }
#endif

    if (strstr((char*)extensions, "GL_OES_mapbuffer")) {
        glMapBufferOES = (PFNGLMAPBUFFEROESPROC) eglGetProcAddress("glMapBufferOES");
        glUnmapBufferOES = (PFNGLUNMAPBUFFEROESPROC) eglGetProcAddress("glUnmapBufferOES");
        glGetBufferPointervOES = (PFNGLGETBUFFERPOINTERVOESPROC) eglGetProcAddress("glGetBufferPointervOES");
    }
    
    if (strstr((char*)extensions, "GL_OES_vertex_array_object")) {
        glBindVertexArrayOES = (PFNGLBINDVERTEXARRAYOESPROC) eglGetProcAddress("glBindVertexArrayOES");
        glDeleteVertexArraysOES = (PFNGLDELETEVERTEXARRAYSOESPROC) eglGetProcAddress("glDeleteVertexArraysOES");
        glGenVertexArraysOES = (PFNGLGENVERTEXARRAYSOESPROC) eglGetProcAddress("glGenVertexArraysOES");
        glIsVertexArrayOES = (PFNGLISVERTEXARRAYOESPROC) eglGetProcAddress("glIsVertexArrayOES");
    }
}

#else

void c3d_gl_load_extensions() {
    // nothing to load, use predefined extensions, i.e. iOS
}

#endif


