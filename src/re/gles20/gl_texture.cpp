#include "re/gles20/gl_texture.h"

#include "io/memory_stream.h"

using namespace gles20;

gl_texture::gl_texture() {
    glGenTextures(1, &_tex_id);
    
    // FIXME:
    glBindTexture(GL_TEXTURE_2D, _tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

gl_texture::~gl_texture() {
    glDeleteTextures(1, &_tex_id);
}

bool gl_texture::load(data_stream* ds, int format, int color, int level) {
    glBindTexture(GL_TEXTURE_2D, _tex_id);
    glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 static_cast<memory_stream*>(ds)->address());
    return true;
}
