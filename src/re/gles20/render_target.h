#ifndef _GLES20_RENDER_TARGET_H
#define _GLES20_RENDER_TARGET_H

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include "../render_target.h"

namespace gles20 {

    class render_frame {
    private:
        GLuint _render_frame;
        GLuint _color_buffer;
        GLuint _depth_buffer;
    };

};
#endif