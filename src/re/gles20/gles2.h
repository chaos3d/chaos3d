#ifdef __APPLE__
#include <TargetConditionals.h>

#if TARGET_OS_IPHONE
// iOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#else
// Mac OS
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#define C3DGL_EXPORT_SDK_API 1

#endif

// PowerVR SDK for other platforms
//#elif defined(ANDROID)
#else
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#define C3DGL_EXPORT_SDK_API 1

#endif

extern void c3d_gl_load_extensions();

#if defined(C3DGL_EXPORT_SDK_API)

// GL_EXT_texture_storage
extern PFNGLTEXSTORAGE1DEXTPROC glTexStorage1DEXT;
extern PFNGLTEXSTORAGE2DEXTPROC glTexStorage2DEXT;
extern PFNGLTEXSTORAGE3DEXTPROC glTexStorage3DEXT;
extern PFNGLTEXTURESTORAGE1DEXTPROC glTexureStorage1DEXT;
extern PFNGLTEXTURESTORAGE1DEXTPROC glTexureStorage2DEXT;
extern PFNGLTEXTURESTORAGE1DEXTPROC glTexureStorage3DEXT;

// GL_OES_mapbuffer
extern PFNGLMAPBUFFEROESPROC glMapBufferOES;
extern PFNGLUNMAPBUFFEROESPROC glUnmapBufferOES;
extern PFNGLGETBUFFERPOINTERVOESPROC glGetBufferPointervOES;

// GL_OES_vertex_array_object
extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES;
extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES;
extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES;
extern PFNGLISVERTEXARRAYOESPROC glIsVertexArrayOES;

#endif //EXPORT_SDK_API

#ifdef DEBUG
#include <iostream>
#include <iomanip>

#define GLNOERROR \
    do { int ret = glGetError(); \
        if(ret != GL_NO_ERROR) { \
            std::cout << "gl error:" << std::hex << ret << std::endl; \
        } \
        assert(ret == GL_NO_ERROR); \
    } while(0)

#define GLASSERT0
#define GLASSERT1(exp)

#else // NO DEBUG
#define GLNOERROR
#define GLASSERT0
#define GLASSERT1(exp)

#endif // END DEBUG

#undef EXTERN
