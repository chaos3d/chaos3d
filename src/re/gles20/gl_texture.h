#ifndef _GLES2_GLTEXTURE_H
#define _GLES2_GLTEXTURE_H

#include "re/gles20/gles2.h"
#include "re/texture.h"

namespace gles20 {
    class gl_texture : public texture {
    public:
        gl_texture();
        virtual ~gl_texture();
        
        virtual bool load(data_stream*, int format, int color, int level = 0) override;
        
        GLuint tex_id() const { return _tex_id; }
    private:
        GLuint _tex_id;
    };
}

#endif