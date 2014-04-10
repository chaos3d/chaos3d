#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

//#include <OpenGLES/ES2/gl.h>
//#include <OpenGLES/ES2/glext.h>

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

#else
#define GLNOERROR
#define GLASSERT0
#define GLASSERT1(exp)
#endif